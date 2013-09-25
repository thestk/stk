/******************************************/
/*
  RtAudio.cpp
  Realtime Sound I/O Object for STK
  by Gary P. Scavone, 1998-2000.

  The sound output sections of this object
  were originally based in part on code by
  Doug Scott (SGI), Tim Stilson (Linux),
  and R. Marsanyi (DirectSound).  The latest
  DirectSound code was re-written by Dave
  Chisholm at CCRMA.

  This object provides a standard API
  across all platforms for STK realtime
  audio input/output.  Multi-channel
  support is supported when provided by
  the soundcard.

  Only 16-bit integer input/output
  routines are written for the moment
  though it would be simple to overload
  the methods for other data types.
*/
/******************************************/

#include "RtAudio.h"

#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))

#define NUM_FRAGMENTS 4

RtAudio :: RtAudio(int channels, MY_FLOAT srate, const char *mode, int device)
{
  ALconfig audio_port_config;
  long queue_size;
  ALpv pvs[2];
  char msg[256];

  // initialize resources
  audio_port_out = 0;
  audio_port_in = 0;

  // check mode string
  if (strcmp(mode,"play") && strcmp(mode,"record") && strcmp(mode,"duplex")) {
    sprintf(msg, "RtAudio: constructor parameter 'mode' must be play, record, or duplex only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  /* Create ALconfig structure */
  audio_port_config = alNewConfig();
  if (!audio_port_config) {
    sprintf(msg,"RtAudio: Couldn't create SGI ALconfig: %s\n",
            alGetErrorString(oserror()));
    throw StkError(msg, StkError::SOUNDCARD_CONTROL);
  }

  /* Configure channels */
  stk_chans = channels;
  if(alSetChannels(audio_port_config, stk_chans) < 0) {
    sprintf(msg,"RtAudio: SGI error configuring %d channels: %s\n",
            channels, alGetErrorString(oserror()));
    throw StkError(msg, StkError::SOUNDCARD_CAPS);
  }

  /* Size the output queue */
  queue_size = RT_BUFFER_SIZE * NUM_FRAGMENTS; // in sample frames
  if(alSetQueueSize(audio_port_config, queue_size) < 0) {
    sprintf(msg,"RtAudio: SGI error configuring output queue size: %s\n",
            alGetErrorString(oserror()));
    throw StkError(msg, StkError::SOUNDCARD_CONTROL);
  }

  if ( !strcmp(mode,"play") || !strcmp(mode,"duplex") ) { // playback only

    /* Open the output audio port */
    audio_port_out = alOpenPort("STK output port", "w", audio_port_config);
    if(!audio_port_out) {
      sprintf(msg,"RtAudio: SGI error ... cannot initialize output audio port: %s\n",
              alGetErrorString(oserror()));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    /* Set sample rate parameters */
    pvs[0].param = AL_MASTER_CLOCK;
    pvs[0].value.i = AL_CRYSTAL_MCLK_TYPE;
    pvs[1].param = AL_RATE;
    pvs[1].value.ll = alDoubleToFixed((double)srate);
    if (alSetParams(AL_DEFAULT_OUTPUT, pvs, 2) < 0) { /* set output SR */
      sprintf(msg,"RtAudio: SGI error ... cannot set sample rate parameters: %s\n",
              alGetErrorString(oserror()));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    /* Tell port to accept refill at buffers - 1 */
    alSetFillPoint(audio_port_out, RT_BUFFER_SIZE * (NUM_FRAGMENTS - 1));
  }
  else if ( !strcmp(mode,"record")  || !strcmp(mode,"duplex") ) { // record only

    /* Open the input audio port */
    audio_port_in = alOpenPort("STK input port", "r", audio_port_config);
    if(!audio_port_in) {
      sprintf(msg,"RtAudio: SGI error ... cannot initialize input audio port: %s\n",
              alGetErrorString(oserror()));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    /* Set sample rate parameters */
    pvs[0].param = AL_MASTER_CLOCK;
    pvs[0].value.i = AL_CRYSTAL_MCLK_TYPE;
    pvs[1].param = AL_RATE;
    pvs[1].value.ll = alDoubleToFixed((double)srate);
    if (alSetParams(AL_DEFAULT_INPUT, pvs, 2) < 0) { /* set input SR */
      sprintf(msg,"RtAudio: SGI error ... cannot set sample rate parameters: %s\n",
              alGetErrorString(oserror()));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    /* tell port to accept refill at buffers - 1 */
    alSetFillPoint(audio_port_in, 0);  
  }

  alFreeConfig(audio_port_config);
  audio_port_config = 0;
}

RtAudio :: ~RtAudio()
{
  if (audio_port_out) alClosePort(audio_port_out);
  audio_port_out=0;

  if (audio_port_in) alClosePort(audio_port_in);
  audio_port_in=0;
}

int RtAudio :: playBuffer(INT16 *buf, int bufsize)
{
    alWriteFrames(audio_port_out, buf, bufsize/stk_chans);
    return 0;
}

int RtAudio :: recordBuffer(INT16 *buf, int bufsize)
{
    alReadFrames(audio_port_in, buf, bufsize/stk_chans);
    return 0;
}


/* Linux ALSA Sound API here */

#elif (defined(__STK_REALTIME_) && defined(__ALSA_API_))

RtAudio :: RtAudio(int channels, MY_FLOAT srate, const char *mode, int device)
{
  int card, dev, err, nChoices = 0;
  int data_format, default_card;
  unsigned int mask;
  snd_pcm_channel_params_t params;
	struct snd_ctl_hw_info info;
	snd_pcm_info_t pcminfo;
	snd_pcm_channel_info_t chninfo;
  snd_pcm_channel_setup_t setup;
  snd_ctl_t *chandle;
  bool print_list = FALSE;
  char msg[256];

  // check mode string
  if (strcmp(mode,"play") && strcmp(mode,"record") && strcmp(mode,"duplex")) {
    sprintf(msg, "RtAudio: constructor parameter 'mode' must be play, record, or duplex only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  if (channels < 1) {
    sprintf(msg, "RtAudio: number of channels = %d not supported!\n", channels);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // check to make sure we have card(s) and/or ALSA drivers available
  mask = snd_cards_mask();
  if (!mask) {
    sprintf(msg, "RtAudio: no ALSA soundcards reported available.\n");
    throw StkError(msg, StkError::SOUNDCARD_NOT_FOUND);
  }

  ohandle = 0;
  ihandle = 0;
  outbuf = 0;
  inbuf = 0;

  if (!strcmp(mode, "play"))
    direction = SND_PCM_INFO_PLAYBACK;
  else if (!strcmp(mode, "record"))
    direction = SND_PCM_INFO_CAPTURE;
  else
    direction = SND_PCM_INFO_DUPLEX;

  // The proliferation of multichannel soundcards carries with it a wide range
  // of special requirements. Often, a card will need to be fed a special data
  // or channel format.  So, we need to probe the available card(s) to determine
  // these requirements.  If no device is specified as a constructor argument,
  // I'll start probing at the default card number (which can be set with the
  // ALSA_CARD environment variable) and then any other existing cards until I
  // find a device that will meet our needs (or not).  If a specified device 
  // does not exist or will not work, I'll print a list of available audio
  // devices and let the user select one.
  if (device == -1) {
    default_card = snd_defaults_pcm_card();
  }
  else { // check device specified as argument
    if (!(mask & (1<<device))) {
      default_card = 0;
      print_list = TRUE;
      printf("\n");
    }
    else {
      default_card = device;
    }
  }

  card = default_card;
  while (card<SND_CARDS) {
    if (mask & (1<<card)) {
      if ((err = snd_ctl_open(&chandle, card)) < 0) {
        fprintf(stderr,"RtAudio: ALSA error on control open (%d): %s\n",
                card, snd_strerror(err));
        continue;
      }
      if ((err = snd_ctl_hw_info(chandle, &info)) < 0) {
        fprintf(stderr,"RtAudio: ALSA error on control hardware info (%d): %s\n",
                card, snd_strerror(err));
        snd_ctl_close(chandle);
        continue;
      }
      for (dev=0; dev<(int)info.pcmdevs; dev++) {
        /* get information for each device */
        if ((err = snd_ctl_pcm_info(chandle, dev, &pcminfo)) < 0) {
          fprintf(stderr,"RtAudio: ALSA error on control PCM info (%d): %s\n",
                  card, snd_strerror(err));
          continue;
        }
        if (pcminfo.flags & direction) { // a device exists for the given mode
          if ( (direction == SND_PCM_INFO_PLAYBACK) ||
               (direction == SND_PCM_INFO_DUPLEX) ) {
            if ((err=snd_pcm_open(&ohandle, card, dev, SND_PCM_OPEN_PLAYBACK))!=0) {
              fprintf(stderr,"RtAudio: ALSA PCM playback open error (%d): %s\n",
                      card, snd_strerror(err));
              continue;
            }
            // we have a device open ... get the channel information (direction specific)
            memset(&chninfo, 0, sizeof(chninfo));
            chninfo.channel = SND_PCM_CHANNEL_PLAYBACK;
            if ((err = snd_pcm_channel_info(ohandle, &chninfo)) < 0) {
              fprintf(stderr,"RtAudio: ALSA error on PCM playback info (%d): %s\n",
                      card, snd_strerror(err));
              snd_pcm_close(ohandle);
              continue;
            }
            // check number of channels and sample rate
            if ((chninfo.max_voices >= channels) && (chninfo.min_rate <= (int)srate)
                && (chninfo.max_rate >= (int)srate)) { // this card will work 
              dev_ochans = (channels >= chninfo.min_voices) ? channels : chninfo.min_voices;
              if (direction == SND_PCM_INFO_PLAYBACK) {
                // playback only ... jump out of loop and proceed
                // duplex case continue to capture check
                if (print_list) {
                  printf("Audio Card %d, Device %d: %s\n", card, dev, info.name);
                  nChoices++;
                  snd_pcm_close(ohandle);
                  continue;
                }
                goto have_good_device;
              }
            } else { // this device won't work
              snd_pcm_close(ohandle);
              continue;
            }
          }
          if ( (direction == SND_PCM_INFO_CAPTURE) ||
              (direction == SND_PCM_INFO_DUPLEX) ) {
            if ((err=snd_pcm_open(&ihandle, card, dev, SND_PCM_OPEN_CAPTURE))!=0) {
              fprintf(stderr,"RtAudio: ALSA PCM capture open error (%d): %s\n",
                      card, snd_strerror(err));
              if (direction == SND_PCM_INFO_DUPLEX)
                snd_pcm_close(ohandle);
              continue;
            }
            // we have a device open ... get the channel information (direction specific)
            memset(&chninfo, 0, sizeof(chninfo));
            chninfo.channel = SND_PCM_CHANNEL_CAPTURE;
            if ((err = snd_pcm_channel_info(ihandle, &chninfo)) < 0) {
              fprintf(stderr,"RtAudio: ALSA error on PCM capture info (%d): %s\n",
                      card, snd_strerror(err));
              snd_pcm_close(ihandle);
              if (direction == SND_PCM_INFO_DUPLEX)
                snd_pcm_close(ohandle);
              continue;
            }
            // check number of channels and sample rate
            if ((chninfo.max_voices >= channels) && (chninfo.min_rate <= (int)srate)
                && (chninfo.max_rate >= (int)srate)) { // this card will work 
              dev_ichans = (channels >= chninfo.min_voices) ? channels : chninfo.min_voices;
              if (print_list) {
                printf("Audio Card %d, Device %d: %s\n", card, dev, info.name);
                nChoices++;
                snd_pcm_close(ihandle);
                if (direction == SND_PCM_INFO_DUPLEX)
                  snd_pcm_close(ohandle);
                continue;
              }
              goto have_good_device; // jump out of loop and proceed
            } else { // this device won't work
              snd_pcm_close(ihandle);
              if (direction == SND_PCM_INFO_DUPLEX)
                snd_pcm_close(ohandle);
            }
          }
        }
      }
    }
    if (default_card == 0) card++;
    else { // default card != 0, now start with card 0 and keep searching
      if (card == default_card) card = 0; // first time only
      else {
        card++;
        if (card == default_card) card++; // skip over default card
      }
    }
  }

  if (print_list && nChoices) {
    char choice[16];
    printf("\nType an audio card number from above: ");
    fgets(choice, 16, stdin);
    card = atoi(choice);
    printf("Select a device for the same card: ");
    fgets(choice, 16, stdin);
    printf("\n");
    dev = atoi(choice);
    // re-open the device(s)
    if ( (direction == SND_PCM_INFO_PLAYBACK) ||
         (direction == SND_PCM_INFO_DUPLEX) ) {
      if ((err=snd_pcm_open(&ohandle, card, dev, SND_PCM_OPEN_PLAYBACK))!=0) {
        sprintf(msg, "RtAudio: ALSA PCM playback open error (%d:%d): %s\n",
                card, dev, snd_strerror(err));
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }
    }
    if ( (direction == SND_PCM_INFO_CAPTURE) ||
        (direction == SND_PCM_INFO_DUPLEX) ) {
      if ((err=snd_pcm_open(&ihandle, card, dev, SND_PCM_OPEN_CAPTURE))!=0) {
        sprintf(msg, "RtAudio: ALSA PCM capture open error (%d:%d): %s\n",
                card, dev, snd_strerror(err));
        if (direction == SND_PCM_INFO_DUPLEX)
          snd_pcm_close(ohandle);
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }
    }
    goto have_good_device;
  }

  // if we got here, no devices were found to meet the requested functionality
  sprintf(msg, "RtAudio: no ALSA device found for requested service!\n");
  throw StkError(msg, StkError::SOUNDCARD_CAPS);

 have_good_device: // the current value of card and dev are what we will use

  // The hardware sometimes requires more channels of data than we necessarily
  // want to work with in STK.  In fact, sometimes the hardware requires a
  // different number of output channels than input channels.  Thus, we need to
  // remember all values.
  stk_chans = channels;

  // If duplex mode, the format will come from the capture device info.  I'm assuming
  // that both directions will have the same data format.
  if (chninfo.formats & SND_PCM_FMT_S16_LE) {
    data_format = SND_PCM_SFMT_S16_LE;
    bytes_per_sample = 2;
  }
  else if (chninfo.formats & SND_PCM_FMT_S32_LE) {
    data_format = SND_PCM_SFMT_S32_LE;
    bytes_per_sample = 4;
  }
  else {
    sprintf(msg, "RtAudio: only ALSA S16_LE and S32_LE data formats at the moment!\n");
    throw StkError(msg);
  }

  // global channel parameters (not direction specific)
  memset(&params, 0, sizeof(params));
  params.mode=SND_PCM_MODE_BLOCK;
  params.stop_mode=SND_PCM_STOP_ROLLOVER; 
  params.buf.block.frags_max=3;
  params.buf.block.frags_min=1;
  params.format.interleave=1;
  params.format.format=data_format; // this should depend on the card, unless we use the plugin
  params.format.rate=(int)srate;

  // We need to set the channel parameters, flush, and prepare for each direction of use.
  // Thus, if doing duplex, we need to do these for both directions.  First do it for
  // playback and duplex.
  if ((direction == SND_PCM_INFO_PLAYBACK) || (direction == SND_PCM_INFO_DUPLEX)) {
    params.channel=SND_PCM_CHANNEL_PLAYBACK;
    params.start_mode=SND_PCM_START_FULL;
    params.format.voices=dev_ochans;
    // The fragsize is in bytes per frame = RT_BUFFER_SIZE * dev_chans * bytes_per_sample.
    // ALSA requires that all reads and write be done in fragsize increments.
    ofragsize = (int) (dev_ochans * bytes_per_sample * RT_BUFFER_SIZE);
    params.buf.block.frag_size=ofragsize;
    if ((err=snd_pcm_channel_params(ohandle, &params))!=0) {
      // try to close what was opened!
      snd_pcm_close(ohandle);
      sprintf(msg, "RtAudio: Cannot set ALSA audio device parameters for playback: %s\n",
              snd_strerror(err));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    // now check the actual values, because sometimes they are not what we requested
    memset(&setup, 0, sizeof(setup));
    setup.channel=SND_PCM_CHANNEL_PLAYBACK;
    if ((err=snd_pcm_channel_setup(ohandle, &setup))<0) {
      snd_pcm_close(ohandle);
      sprintf(msg, "RtAudio: Cannot get ALSA audio device setup info: %s\n",
              snd_strerror(err));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    } else {
      if (setup.format.rate != (int) srate) {
        // try to close what was opened!
        snd_pcm_close(ohandle);
        sprintf(msg, "RtAudio: Soundcard doesn't seem to support requested sample rate: %.2f!\n",
                srate);
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }
      if (setup.buf.block.frag_size != ofragsize) ofragsize = setup.buf.block.frag_size;
    }

    // allocate and clear the output buffer
    outbuf = (unsigned char*) new char[ofragsize];
    memset(outbuf, 0, ofragsize);

    // I'm not sure what this does and I'm not sure it actually helps.
    if ((err=snd_pcm_playback_flush(ohandle))!=0) {
      // try to free and close what was opened!
      snd_pcm_close(ohandle);
      if (outbuf) delete [] outbuf;
      sprintf(msg, "RtAudio: Cannot flush ALSA channel buffers for playback!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    if ((err=snd_pcm_channel_prepare(ohandle, params.channel))!=0) {
      // try to close what was opened!
      snd_pcm_close(ohandle);
      if (outbuf) delete [] outbuf;
      sprintf(msg, "RtAudio: Cannot prepare ALSA channel for playback!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }
  }

  // Now do it for capture and duplex.
  if ((direction == SND_PCM_INFO_CAPTURE) || (direction == SND_PCM_INFO_DUPLEX)) {
    params.channel=SND_PCM_CHANNEL_CAPTURE;
    params.start_mode=SND_PCM_START_DATA;
    params.format.voices=dev_ichans;
    // The fragsize is in bytes per frame = RT_BUFFER_SIZE * dev_chans * bytes_per_sample.
    // ALSA requires that all reads and write be done in fragsize increments.
    ifragsize = (int) (dev_ichans * bytes_per_sample * RT_BUFFER_SIZE);
    params.buf.block.frag_size=ifragsize;
    if ((err=snd_pcm_channel_params(ihandle, &params))!=0) {
      // try to close what was opened!
      snd_pcm_close(ihandle);
      if (direction == SND_PCM_INFO_DUPLEX) {
        if (outbuf) delete [] outbuf;
        snd_pcm_close(ohandle);
      }
      sprintf(msg, "RtAudio: Cannot set ALSA audio device parameters for capture: %s\n",
              snd_strerror(err));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    // now check the actual values, because sometimes they are not what we requested
    memset(&setup, 0, sizeof(setup));
    setup.channel=SND_PCM_CHANNEL_CAPTURE;
    if ((err=snd_pcm_channel_setup(ihandle, &setup))<0) {
      // try to close what was opened!
      snd_pcm_close(ihandle);
      if (direction == SND_PCM_INFO_DUPLEX) {
        if (outbuf) delete [] outbuf;
        snd_pcm_close(ohandle);
      }
      sprintf(msg, "RtAudio: Cannot get ALSA audio device setup info: %s\n",
              snd_strerror(err));
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    } else {
      if (setup.format.rate != (int) srate) {
        // try to close what was opened!
        snd_pcm_close(ihandle);
        if (direction == SND_PCM_INFO_DUPLEX) {
          if (outbuf) delete [] outbuf;
          snd_pcm_close(ohandle);
        }
        sprintf(msg, "RtAudio: Soundcard doesn't seem to support requested sample rate: %.2f!\n",
                srate);
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }

      if (setup.buf.block.frag_size != ifragsize) ifragsize = setup.buf.block.frag_size;
    }

    // allocate and clear the input buffer
    inbuf = (unsigned char*) new char[ifragsize];
    memset(inbuf, 0, ifragsize);

    if ((err=snd_pcm_capture_flush(ihandle))!=0) {
      // try to close what was opened!
      snd_pcm_close(ihandle);
      if (inbuf) delete [] inbuf;
      if (direction == SND_PCM_INFO_DUPLEX) {
        if (outbuf) delete [] outbuf;
        snd_pcm_close(ohandle);
      }
      sprintf(msg, "RtAudio: Cannot flush ALSA channel buffers for capture!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    if ((err=snd_pcm_channel_prepare(ihandle, params.channel))!=0) {
      // try to close what was opened!
      snd_pcm_close(ihandle);
      if (inbuf) delete [] inbuf;
      if (direction == SND_PCM_INFO_DUPLEX) {
        if (outbuf) delete [] outbuf;
        snd_pcm_close(ohandle);
      }
      sprintf(msg, "RtAudio: Cannot prepare ALSA channel for capture!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }
  }
}

RtAudio :: ~RtAudio()
{
  if ((direction == SND_PCM_INFO_PLAYBACK) || (direction == SND_PCM_INFO_DUPLEX)) {
    snd_pcm_playback_drain(ohandle);
    snd_pcm_close(ohandle);
  }
  if ((direction == SND_PCM_INFO_CAPTURE) || (direction == SND_PCM_INFO_DUPLEX)) {
    snd_pcm_close(ihandle);
  }
  if (outbuf) delete [] outbuf;
  if (inbuf) delete [] inbuf;
}

int RtAudio :: playBuffer(INT16 *buf, int bufsize)
{
  // The argument bufsize is the number of audio samples (INT16s) in buf.
  int err, i;
  static char msg[256];
  unsigned char *temp;
  static int channel = 1;
  static int counter = 0;
  static int extra_chans = dev_ochans - stk_chans;

  // performance optimization occurs when the following conditions are met
  if ((extra_chans == 0) && (bytes_per_sample == 2) && (bufsize * 2 == ofragsize)) {
    // we don't need to use outbuf
    if ((err=snd_pcm_write(ohandle, buf, ofragsize))!=ofragsize) {
      sprintf(msg, "RtAudio: ALSA audio write error!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }
  } else {
    for (i=0; i<bufsize; i++) {
      // write the i-th sample to outbuf
      temp = (unsigned char *) &buf[i];
      // shift data to the two highest bytes of each output sample
      counter += bytes_per_sample-2;
      outbuf[counter++] = temp[0];
      outbuf[counter++] = temp[1];
      channel++;

      if (channel > stk_chans) {
        // because we created outbuf with calloc(), we shouldn't have to write zeros here
        counter += bytes_per_sample * extra_chans;
        channel = 1;
      }

      if (counter >= ofragsize) {
        if ((err=snd_pcm_write(ohandle, outbuf, ofragsize))!=ofragsize) {
          sprintf(msg, "RtAudio: ALSA audio write error!\n");
          throw StkError(msg, StkError::SOUNDCARD_CONTROL);
        }
        counter = 0;
      }
    }
  }

  return 0;
}

int RtAudio :: recordBuffer(INT16 *buf, int bufsize)
{
  // The argument bufsize is the number of audio samples (INT16s) in buf.
  int err, i;
  static char msg[256];
  unsigned char *temp;
  static int channel = 1;
  static int counter = 0;
  static int extra_chans = dev_ichans - stk_chans;

  // performance optimization occurs when the following conditions are met
  if ((extra_chans == 0) && (bytes_per_sample == 2) && (bufsize * 2 == ifragsize)) {
    // we don't need to use inbuf
    if ((err=snd_pcm_read(ihandle, buf, ifragsize))!=ifragsize) {
      sprintf(msg, "RtAudio: ALSA audio read error!\n");
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }
  } else {
    for (i=0; i<bufsize; i++) {
      if (counter == 0) {
        if ((err=snd_pcm_read(ihandle, inbuf, ifragsize))!=ifragsize) {
          sprintf(msg, "RtAudio: ALSA audio read error!\n");
          throw StkError(msg, StkError::SOUNDCARD_CONTROL);
        }
      }

      temp = (unsigned char *) &buf[i];
      // take data from the two highest bytes of each input sample
      counter += bytes_per_sample-2;
      temp[0] = inbuf[counter++];
      temp[1] = inbuf[counter++];
      channel++;

      if (channel > stk_chans) {
        // just skip over the extra channels
        counter += bytes_per_sample * extra_chans;
        channel = 1;
      }
      if (counter >= ifragsize) counter = 0;
    }
  }

  return 0;
}


/* Linux OSS Sound API here */

#elif (defined(__STK_REALTIME_) && defined(__OSS_API_))

// Define the maximum number of dsp devices that we'll attempt
// to probe before giving up.
#define MAX_DSP_DEVS 8
#define DAC_NAME "/dev/dsp"
// The number of fragments can be made larger than 2 if the sound
// system performance is poor.
#define NUM_FRAGMENTS 4

RtAudio :: RtAudio(int channels, MY_FLOAT srate, const char *mode, int device)
{
  char device_name[16];
  int fragsize;
  int fragment_size_log;
  int format;
  int chans = channels;
  int speed;
  int i, nChoices = 0;
  bool print_list = FALSE;
  char msg[256];

  // check mode string
  if (strcmp(mode,"play") && strcmp(mode,"record") && strcmp(mode,"duplex")) {
    sprintf(msg, "RtAudio: constructor parameter 'mode' must be play, record, or duplex only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  if (channels < 1) {
    sprintf(msg, "RtAudio: number of channels = %d not supported!\n", channels);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  fragsize = RT_BUFFER_SIZE * channels * 2; // in bytes
  fragment_size_log = (int)(log10((double)fragsize)/log10(2.0));
  long temp = ((long) NUM_FRAGMENTS << 16) + fragment_size_log;

  // /dev/dsp should be a link to the default pcm device under OSS
  strcpy(device_name, DAC_NAME);

  // The OSS API doesn't really give us a means for probing the
  // capabilities of devices.  Thus, we'll just pursue a brute
  // force method of opening devices until we either find something
  // that doesn't complain or we have to give up.  We'll start with
  // the default device, then try /dev/dsp0, /dev/dsp1, etc...

  if (device != -1) {
    // start with device specified as argument
    sprintf(device_name, "%s%d", DAC_NAME, device);
  }

  for (i=0; i<=MAX_DSP_DEVS; i++) {

    // if the default device doesn't work, try some others
    if (i > 0) sprintf(device_name, "%s%d", DAC_NAME, i-1);

    if (device != -1 && i == 1) {
      // the specified device didn't work ... now print other options
      print_list = TRUE;
      printf("\n");
    }

    if (!strcmp(mode,"play")) { // playback only
      if ((audio_fd = open(device_name, O_WRONLY, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        if (errno == EBUSY)
          fprintf(stderr,"RtAudio: OSS PCM playback device (%s) is busy and cannot be opened.\n",
                  device_name);
        continue;
      }
    }
    else if (!strcmp(mode,"record")) { // record only
      if ((audio_fd = open(device_name, O_RDONLY, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        if (errno == EBUSY)
          fprintf(stderr,"RtAudio: OSS PCM record device (%s) is busy and cannot be opened.\n",
                  device_name);
        continue;
      }
    }
    else if (!strcmp(mode,"duplex")) { // duplex mode
      if ((audio_fd = open(device_name, O_RDWR, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        if (errno == EBUSY)
          fprintf(stderr,"RtAudio: OSS PCM device (%s) is busy and cannot be opened for duplex operation.\n",
                  device_name);
        continue;
      }
      int caps;
      if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &caps)) {
        close(audio_fd);
        fprintf(stderr,"RtAudio: OSS error getting device (%s) capabilities for duplex operation.\n",
                device_name);
        continue;
      }
      if (!(caps & DSP_CAP_DUPLEX)) {
        close(audio_fd);
        fprintf(stderr,"RtAudio: OSS reports device (%s) does not support duplex operation.\n",
                device_name);
        continue;
      }
      if (ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, 0)) {
        close(audio_fd);
        fprintf(stderr,"RtAudio: OSS error setting device (%s) for duplex operation.\n",
                device_name);
        continue;
      }
    }

    // Setup the number and size of the fragments
    if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &temp)) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error setting fragment size for device (%s).\n",
              device_name);
      continue;
    }

    // Setup the data format ... we're only supporting 16-bit little-endian data for now
    format = AFMT_S16_LE;
    if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format) == -1) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error setting data format for device (%s).\n",
              device_name);
      continue;
    }

    // Check the see whether the device supports the requested format
    if (format != AFMT_S16_LE) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error ... audio device (%s) doesn't support 16-bit signed LE format.\n",
              device_name);
      continue;
    }

    // Setup the number of channels
    if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &chans) == -1) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error setting %d channels on  device (%s).\n",
              channels, device_name);
      continue;
    }

    // Check to see whether the device supports the requested number of channels
    if (chans != channels ) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error ... audio device (%s) doesn't support %d channels.\n",
              device_name, channels);
      continue;
    }

    // Setup the sampling rate
    speed = (int) srate;
    if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed)==-1) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error setting sample rate = %f on  device (%s).\n",
              srate, device_name);
      continue;
    }
    
    // Check to see whether the device supports the requested sample rate
    if (abs(speed - (int)srate) > 100) {
      close(audio_fd);
      fprintf(stderr,"RtAudio: OSS error ... audio device (%s) doesn't support sample rate of %f.\n",
              device_name, srate);
      continue;
    }

    if (print_list) {
      close(audio_fd);
      printf("Audio Device %d: %s\n", i-1, device_name);
      nChoices++;
      continue;
    }

    // If we got here, we found a device that meets our needs.  Return to the caller.
    return;
  }

  // If we got here and print_list = TRUE, then we need to ask the user to specify
  // a device from the printed list.  Then we have to go through the whole process
  // of opening and checking the device capabilities again, as we did above.  We
  // can't assume the user always types a valid device number.  We can thank OSS
  // for the kludgy-ness of this whole process.
  if (print_list && nChoices ) {
    char choice[16];
    printf("\nType an audio device number from above: ");
    fgets(choice, 16, stdin);
    i = atoi(choice);
    printf("\n");
    sprintf(device_name, "%s%d", DAC_NAME, i);

    // Now redo the above with the specified card.
    if (!strcmp(mode,"play")) { // playback only
      if ((audio_fd = open(device_name, O_WRONLY, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        sprintf(msg, "RtAudio: OSS PCM playback device (%s) cannot be opened.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CONTROL);
      }
    }
    else if (!strcmp(mode,"record")) { // record only
      if ((audio_fd = open(device_name, O_RDONLY, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        sprintf(msg, "RtAudio: OSS PCM record device (%s) cannot be opened.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CONTROL);
      }
    }
    else if (!strcmp(mode,"duplex")) { // duplex mode
      if ((audio_fd = open(device_name, O_RDWR, 0)) == -1) {
        // Open device failed ... either busy or doesn't exist
        sprintf(msg, "RtAudio: OSS PCM device (%s) cannot be opened for duplex operation.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CONTROL);
      }
      int caps;
      if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &caps)) {
        close(audio_fd);
        sprintf(msg, "RtAudio: OSS error getting device (%s) capabilities for duplex operation.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }
      if (!(caps & DSP_CAP_DUPLEX)) {
        close(audio_fd);
        sprintf(msg, "RtAudio: OSS reports device (%s) does not support duplex operation.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CAPS);
      }
      if (ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, 0)) {
        close(audio_fd);
        sprintf(msg, "RtAudio: OSS error setting device (%s) for duplex operation.\n",
                device_name);
        throw StkError(msg, StkError::SOUNDCARD_CONTROL);
      }
    }

    // Setup the number and size of the fragments
    if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &temp)) {
      close(audio_fd);
      sprintf(msg, "RtAudio: OSS error setting fragment size for device (%s).\n",
              device_name);
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    // Setup the data format ... we're only supporting 16-bit little-endian data for now
    format = AFMT_S16_LE;
    if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format) == -1) {
      close(audio_fd);
      sprintf(msg,"RtAudio: OSS error setting data format for device (%s).\n",
              device_name);
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    // Check the see whether the device supports the requested format
    if (format != AFMT_S16_LE) {
      close(audio_fd);
      sprintf(msg,"RtAudio: OSS error ... audio device (%s) doesn't support 16-bit signed LE format.\n",
              device_name);
      throw StkError(msg, StkError::SOUNDCARD_CAPS);
    }

    // Setup the number of channels
    if (ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &chans) == -1) {
      close(audio_fd);
      sprintf(msg, "RtAudio: OSS error setting %d channels on  device (%s).\n",
              channels, device_name);
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }

    // Check to see whether the device supports the requested number of channels
    if (chans != channels ) {
      close(audio_fd);
      sprintf(msg, "RtAudio: OSS error ... audio device (%s) doesn't support %d channels.\n",
              device_name, channels);
      throw StkError(msg, StkError::SOUNDCARD_CAPS);
    }

    // Setup the sampling rate
    speed = (int) srate;
    if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed)==-1) {
      close(audio_fd);
      sprintf(msg, "RtAudio: OSS error setting sample rate = %f on  device (%s).\n",
              srate, device_name);
      throw StkError(msg, StkError::SOUNDCARD_CONTROL);
    }
    
    // Check to see whether the device supports the requested sample rate
    if (abs(speed - (int)srate) > 100) {
      close(audio_fd);
      sprintf(msg,"RtAudio: OSS error ... audio device (%s) doesn't support sample rate of %f.\n",
              device_name, srate);
      throw StkError(msg, StkError::SOUNDCARD_CAPS);
    }

    // If we got here, the specified device works ... return to the caller.
    return;
  }

  // If we got here, no device was found to meet the requested functionality
  sprintf(msg, "RtAudio: No OSS device found for requested service!\n");
  throw StkError(msg, StkError::SOUNDCARD_CAPS);
}

RtAudio :: ~RtAudio()
{
  if (audio_fd) close(audio_fd);
  audio_fd=0;
}

int RtAudio :: playBuffer(INT16 *buf, int bufsize)
{
  // The argument bufsize is the number of audio samples (INT16s) in buf.
  // The OSS write() routine takes its buffer size in bytes, thus the
  // multiplication by two.  While it is not necessary to call this function
  // with fragsize buffers (which should equal RT_BUFFER_SIZE * channels * 2),
  // its behavior is apparently optimized for such.
  static char msg[256];

  if (write(audio_fd, buf, 2*bufsize) == -1) {
    sprintf(msg, "RtAudio: OSS audio write error!\n");
    throw StkError(msg, StkError::SOUNDCARD_CONTROL);
  }
  return 0;
}

int RtAudio :: recordBuffer(INT16 *buf, int bufsize)
{
  // The argument bufsize is the number of audio samples (INT16s) in buf.
  // The OSS read() routine takes its buffer size in bytes, thus the
  // multiplication by two.  While it is not necessary to call this function
  // with fragsize buffers (which should equal RT_BUFFER_SIZE * channels * 2),
  // its behavior is apparently optimized for such.
  static char msg[256];

  if (read(audio_fd, buf, 2*bufsize) == -1) {
    sprintf(msg, "RtAudio: OSS audio read error!\n");
    throw StkError(msg, StkError::SOUNDCARD_CONTROL);
  }
  return 0;
}


#elif (defined(__STK_REALTIME_) && defined(__OS_Win_) )
/*
  WINDOZE:

  If anyone needed substantiation as to why Microsoft doesn't deserve
  to exist, just read through the following code and compare it to
  any of the other supported APIs in this file.  Dave, thanks for
  your hard work to get things running better ... it is appreciated.
  But personally, I'm disgusted with Microsoft and their pure
  incompetence (GPS, 10/2000).
 
  AUDIO OUTPUT:
 
  There are several ways to go about handling sound output
  under DirectSound.  We choose to always write new buffers of
  data behind the read pointer of the DS play buffer. This is
  safe (compared to trying to lead the pointer) but inherently
  produces a delay equivalent to the entire sound buffer (plus
  any other delays that Windows provides). Our default parameters
  cause an inherent delay of about 30 ms b/c of this. To change
  this, adjust the NUM_FRAGMENTS definition in RtAudio.h
 
  AUDIO INPUT:
 
  We chose to use a complex but stable scheme of keeping our own internal
  buffer separate from the DS input buffer. This take up more CPU time
  but is safer than using the Sleep() function for critical parts of
  code. Users are then given copies of this data (or pointers into our
  actual internal buffer, if they use the overloaded version of
  recordBuffer(INT16**) A separate thread (under control of the WinMM
  timer) takes care of transfering data from the DS buffer to our own buffer.
  You can expect a total delay of about 100 - 150 ms for fully duplexed
  audio - it seems impossible to get directx to perform any better than this.
 
  In addition, the windows version has stopPlay, startPlay, startRecord
  and stopRecord methods b/c DirectX does not automatically stop I/O if
  no fresh data is sent.
 
  The DirectSoundCapture API is only available with DirectX versions
  5.0 and higher.
 */

RtAudio :: RtAudio(int channels, MY_FLOAT srate, const char *mode, int device)
{
  HRESULT result;
  BYTE* audioPtr;
  DWORD dataLen;
  HWND hWnd = GetForegroundWindow();
	WAVEFORMATEX waveFormat;
	LPGUID directSoundGuid = NULL;
	int i, devNum = 0;

  // Initialize the DirectSound object and buffer pointers to NULL
  directSoundObject = NULL;
  directSoundBuffer = NULL;
  directSoundCaptureObject = NULL;
  directSoundCaptureBuffer = NULL;
  inputBuffer = NULL;

	// Some basic initialization stuff
	sampleRate = srate;
	nextRecordRead = 0;
	nextRecordWrite = 0;
	internalError = false;
	playing = false;
	recording = false;
	numDevices = 0;
	nextWritePos = 0;

  // Define the wave format structure (16-bit PCM, srate, channels)
  ZeroMemory(&waveFormat, sizeof(WAVEFORMATEX));
  waveFormat.wFormatTag = WAVE_FORMAT_PCM;
  waveFormat.nChannels = channels;
  waveFormat.nSamplesPerSec = (unsigned long) srate;
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

  // check mode string
  if (strcmp(mode,"play") && strcmp(mode,"record") && strcmp(mode,"duplex")) {
    sprintf(errormsg, "RtAudio: constructor parameter 'mode' must be play, record, or duplex only.\n");
    throw StkError(errormsg, StkError::FUNCTION_SYNTAX);
  }

  if (channels < 1) {
    sprintf(errormsg, "RtAudio: number of channels = %d not supported!\n", channels);
    throw StkError(errormsg, StkError::FUNCTION_SYNTAX);
  }

	// PLAYBACK INITIALIZATION
	if ( (!strcmp(mode,"play")) || (!strcmp(mode,"duplex")) ) {

    DSBUFFERDESC directSoundBufferDescription;

    if (device == -1) {
      // use default device
      if (FAILED(result = DirectSoundCreate(NULL, &directSoundObject, NULL))) {
        sprintf(errormsg,"RtAudio: Unable to create default direct sound object: %s\n", getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_NOT_FOUND);
      }
      goto have_good_output_device;
    } else {
      // Enumerate through the devices, trying the user specified device first.
      // If that fails, prompt for action.
      if (FAILED(result = DirectSoundEnumerate((LPDSENUMCALLBACK)SoundDeviceEnumCallback, this))) {
        sprintf(errormsg,"RtAudio: Unable to enumerate through sound devices: %s\n", getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_NOT_FOUND);
      }
      if (device >=0 && device < numDevices) {
        // try user specified device if within bounds
        directSoundGuid = devices[device].guid;
        result = DirectSoundCreate(directSoundGuid, &directSoundObject, NULL);
        if (result == DS_OK) goto have_good_output_device;
      }
    }

    printf("\n");
    for (i=0; i<numDevices; i++) {
      printf("Playback Device %d:  %s\n", i, devices[i].description);
    }
    printf("\nPlease type a sound device number from above: ");
    char choice[16];
    fgets(choice, 16, stdin);
    devNum = atoi(choice);
    if (devNum >=0 && devNum < numDevices) {
      directSoundGuid = devices[devNum].guid;
      if (FAILED(result = DirectSoundCreate(directSoundGuid, &directSoundObject, NULL))) {
        sprintf(errormsg,"RtAudio: Unable to create direct sound object: %s\n", getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_CAPS);
      }
    } else {
      throw StkError("RtAudio: Invalid device number specified!",StkError::SOUNDCARD_NOT_FOUND);
    }

    have_good_output_device:

		// Set cooperative level
		if (FAILED(result = directSoundObject->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))) {
			sprintf(errormsg,"RtAudio: Unable to set cooperative level: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

    // Get DS device capabilites
    DSCAPS directSoundCapabilities;
    ZeroMemory(&directSoundCapabilities, sizeof(DSCAPS));
    directSoundCapabilities.dwSize = sizeof(DSCAPS);
    if (FAILED(result = directSoundObject->GetCaps(&directSoundCapabilities))) {
			sprintf(errormsg,"RtAudio: Unable to get DS capabilities: %s\n", getErrorMessage(result));
      throw StkError(errormsg,StkError::SOUNDCARD_CAPS);
    }

		/* Even though we will write to the secondary buffer, we need
       to access the primary buffer to set the correct output format.
       The default is 8-bit, 22 kHz!
    */
		LPDIRECTSOUNDBUFFER directSoundPrimaryBuffer;
		// Setup the DS primary buffer description.
		ZeroMemory(&directSoundBufferDescription, sizeof(DSBUFFERDESC));
		directSoundBufferDescription.dwSize = sizeof(DSBUFFERDESC);
		directSoundBufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
		// Obtain the primary buffer
		if (FAILED(result = directSoundObject->CreateSoundBuffer(&directSoundBufferDescription,
                                                             &directSoundPrimaryBuffer, NULL))) {
			sprintf(errormsg,"RtAudio: Unable to access DS primary buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

    // Set the primary DS buffer sound format.
		if (FAILED(result = directSoundPrimaryBuffer->SetFormat(&waveFormat))) {
			sprintf(errormsg,"RtAudio: Unable to set DS primary buffer format: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

		// Setup the secondary DS buffer description. Try to do it in hardware
		directSoundBufferSize = channels * RT_BUFFER_SIZE * NUM_FRAGMENTS * sizeof(INT16);
		ZeroMemory(&directSoundBufferDescription, sizeof(DSBUFFERDESC));
		directSoundBufferDescription.dwSize = sizeof(DSBUFFERDESC);
		directSoundBufferDescription.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCHARDWARE;
		directSoundBufferDescription.dwBufferBytes = directSoundBufferSize;
		directSoundBufferDescription.lpwfxFormat = &waveFormat;

		// Try to create the secondary DS buffer. If that doesn't work, try for
    // software - if that doesn't work, throw an exception.
		if (FAILED(result = directSoundObject->CreateSoundBuffer(&directSoundBufferDescription, &directSoundBuffer, NULL))) {
			directSoundBufferDescription.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
			if (FAILED(result = directSoundObject->CreateSoundBuffer(&directSoundBufferDescription, &directSoundBuffer, NULL))) {
				sprintf(errormsg,"RtAudio: Unable to create secondary DS buffer: %s\n", getErrorMessage(result));
				throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
			}
		}

		// Get the buffer size
		DSBCAPS dsbcaps;
		dsbcaps.dwSize = sizeof(DSBCAPS);
		directSoundBuffer->GetCaps(&dsbcaps);
		directSoundBufferSize = dsbcaps.dwBufferBytes;

    // Lock the DS buffer
		if (FAILED(result = directSoundBuffer->Lock(0, directSoundBufferSize, (LPLPVOID) &audioPtr, &dataLen, NULL, NULL, 0))) {
			sprintf(errormsg,"RtAudio: Unable to lock DS buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

    // Zero the DS buffer
    ZeroMemory(audioPtr, dataLen);

    // Unlock the DS buffer
		if (FAILED(result = directSoundBuffer->Unlock(audioPtr, dataLen, NULL, 0))) {
			sprintf(errormsg,"RtAudio: Unable to unlock DS buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}
	} // end of play initialization


	// RECORDING INITIALIZATION
  if ( (!strcmp(mode,"record")) || (!strcmp(mode,"duplex")) ) {

    DSCBUFFERDESC directSoundCaptureDescription;

    if (device == -1) {
      // use default device
      if (FAILED(result = DirectSoundCaptureCreate(NULL, &directSoundCaptureObject, NULL))) {
        sprintf(errormsg,"RtAudio: Unable to create default direct sound capture object: %s\n", getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_NOT_FOUND);
      }
      goto have_good_input_device;
    } else {
      // Enumerate through the devices, trying the user specified device first.
      // If that fails, prompt for action.
      if (FAILED(result = DirectSoundCaptureEnumerate((LPDSENUMCALLBACK)SoundDeviceEnumCallback, this))) {
        sprintf(errormsg,"RtAudio: Unable to enumerate through sound capture devices: %s\n", getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_NOT_FOUND);
      }
      if (device >=0 && device < numDevices) {
        // try user specified device if within bounds
        directSoundGuid = devices[device].guid;
        result = DirectSoundCaptureCreate(directSoundGuid, &directSoundCaptureObject, NULL);
        if (result == DS_OK) goto have_good_input_device;
      }
    }

    printf("\n");
    for (i=0; i<numDevices; i++) {
      printf("Capture Device %d:  %s\n", i, devices[i].description);
    }
    printf("\nPlease type a sound device number from above: ");
    char choice[16];
    fgets(choice, 16, stdin);
    devNum = atoi(choice);
    if (devNum >=0 && devNum < numDevices) {
      directSoundGuid = devices[devNum].guid;
      if (FAILED(result = DirectSoundCaptureCreate(directSoundGuid, &directSoundCaptureObject, NULL))) {
        sprintf(errormsg,"RtAudio: Unable to create direct sound capture object: %s\n",getErrorMessage(result));
        throw StkError(errormsg,StkError::SOUNDCARD_CAPS);
      }
    } else {
      throw StkError("RtAudio: Invalid device number specified!\n", StkError::SOUNDCARD_NOT_FOUND);
    }

    have_good_input_device:

    // Setup the DS Capture buffer description
    ZeroMemory(&directSoundCaptureDescription, sizeof(DSCBUFFERDESC));
    directSoundCaptureDescription.dwSize = sizeof(DSCBUFFERDESC);
    directSoundCaptureDescription.dwFlags = 0;
    directSoundCaptureBufferSize = (DWORD)(waveFormat.nAvgBytesPerSec * DS_CAPTURE_BUFFER_SIZE);
    directSoundCaptureDescription.dwBufferBytes = directSoundCaptureBufferSize;
    directSoundCaptureDescription.dwReserved = 0;
    directSoundCaptureDescription.lpwfxFormat = &waveFormat;

    // Create the DS Capture buffer
    if (FAILED(result = directSoundCaptureObject->CreateCaptureBuffer(&directSoundCaptureDescription, &directSoundCaptureBuffer, NULL))) {
			sprintf(errormsg,"RtAudio: Unable to create DS capture buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
    }

    // Lock the DS Capture buffer
    if (FAILED(result = directSoundCaptureBuffer->Lock(0, directSoundCaptureBufferSize, (LPLPVOID) &audioPtr, &dataLen, NULL, NULL, 0))) {
			sprintf(errormsg,"RtAudio: Unable to lock DS capture buffer: %s\n", getErrorMessage(result));
      throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
    }

    // Zero the DS Capture buffer
    ZeroMemory(audioPtr, dataLen);

    // Size and allocate our inputBuffer
    inputBufferSize = (long) (sampleRate * channels * 2 * sizeof(INT16));
    inputBuffer = (BYTE *) new BYTE[inputBufferSize];

    //Zero our internal input buffer
    ZeroMemory(inputBuffer, inputBufferSize);

    // Unlock the DS Capture buffer
    if (FAILED(result = directSoundCaptureBuffer->Unlock(audioPtr, dataLen, NULL, 0))) {
			sprintf(errormsg,"RtAudio: Unable to unlock DS capture buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
    }

    // Start the DS Capture buffer input
    if (FAILED(result = directSoundCaptureBuffer->Start(DSCBSTART_LOOPING))) {
			sprintf(errormsg,"RtAudio: Unable to start DS capture buffer looping: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
    }

    //CALLBACK STUFF HERE
    if ((timerID = timeSetEvent (TIMER_PERIOD, TIMER_RESOLUTION, PeriodicCallbackFn, (DWORD) this, TIME_PERIODIC)) == NULL) {
			throw StkError("RtAudio: Couldn't start periodic timer callback function for audio input.\n",StkError::PROCESS_THREAD);
    }
		recording = true;

  } // end of record initialization

  if ( (!strcmp(mode,"play")) || (!strcmp(mode,"duplex")) ) {
    // Start the DS buffer playback
    if (FAILED(result = directSoundBuffer->Play(0, 0, DSBPLAY_LOOPING ))) {
			sprintf(errormsg,"RtAudio: Unable to start DS buffer looping: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
    }
		playing = true;
  }
}

RtAudio :: ~RtAudio()
{
	// Cleanup the DS buffers
  if (directSoundBuffer)	{
	  directSoundBuffer->Stop();
	  directSoundBuffer->Release();
	  directSoundBuffer = NULL;
	}
  if (directSoundCaptureBuffer) {
	  directSoundCaptureBuffer->Stop();
	  directSoundCaptureBuffer->Release();
	  directSoundCaptureBuffer = NULL;
	}

  // Cleanup the DS objects
  if (directSoundObject) {
	  directSoundObject->Release();
	  directSoundObject = NULL;
	}
  if (directSoundCaptureObject) {
	  directSoundCaptureObject->Release();
	  directSoundCaptureObject = NULL;
	}
	if (timerID) {
		timeKillEvent (timerID);
  }
  if (inputBuffer) {
    delete [] inputBuffer;
    inputBuffer = 0;
  }
}

int RtAudio :: playBuffer(short *buf, int bufsize)
{
	if (!playing) startPlay();

	HRESULT result;
  LPVOID buffer1 = NULL;
  LPVOID buffer2 = NULL;
  DWORD bufferSize1 = 0;
  DWORD bufferSize2 = 0;
  DWORD playPos, safePos;
	int samplesToWrite;

	do {
    // The user may send us a buffer of data that is too large
    // for us to stream in at once, so set up a loop and check
    // the size of their buffer.
		samplesToWrite = bufsize;
		if (samplesToWrite > RT_BUFFER_SIZE  ) {
			samplesToWrite = RT_BUFFER_SIZE ;
		}

		// Find out where the read and "safe write" pointers are.
		if (FAILED(result = directSoundBuffer->GetCurrentPosition(&playPos, &safePos))) {
			sprintf(errormsg,"RtAudio: Unable to get current DS position: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

		if( playPos < nextWritePos ) playPos += directSoundBufferSize; // unwrap offset
		DWORD endWrite = nextWritePos + samplesToWrite * sizeof(INT16);

    //printf("bufsize = %d ", bufsize);
		// Check whether the entire write region is behind the play pointer.
		while ( playPos < endWrite ) {
			// If we are here, then we must wait until the play pointer
			// gets beyond the write region.  To do so, we can use the
      // Sleep() function (which has questionable accuracy).
			// Sleep until safePos catches up. Calculate number of
			// milliseconds to wait as:
			//   time = distance * (milliseconds/second) * fudgefactor /
			//          ((bytes/sample) * (samples/second))
			// A "fudgefactor" less than 1 is used because it was found
			// that sleeping too long was MUCH worse than sleeping for
			// several shorter periods.
			DWORD millis = (DWORD) (((endWrite - playPos) * 900.0) / ( sizeof(INT16) * sampleRate));
      //printf(". ");
			Sleep( millis );

			// Wake up, find out where we are now
			result = directSoundBuffer->GetCurrentPosition( &playPos, &safePos );
			if( result != DS_OK ) return -1;
			if( playPos < nextWritePos ) playPos += directSoundBufferSize; // unwrap offset
		}

	  // Lock free space in the buffer
		result = directSoundBuffer->Lock (nextWritePos, samplesToWrite * sizeof(INT16), &buffer1, &bufferSize1, &buffer2, &bufferSize2, 0);
		if (SUCCEEDED(result)) {
			// Copy the buffer into the DS
			CopyMemory(buffer1, buf, bufferSize1);
			if(NULL != buffer2) CopyMemory(buffer2, buf+(bufferSize1/sizeof(INT16)), bufferSize2);

			// Update our buffer offset and unlock sound buffer
			if (FAILED(directSoundBuffer->Unlock (buffer1, bufferSize1, buffer2, bufferSize2))) {
				throw StkError("RtAudio: Unable to unlock DS buffer\n",StkError::SOUNDCARD_CONTROL);
			}
			nextWritePos = (nextWritePos + bufferSize1 + bufferSize2) % directSoundBufferSize;
		}	else {
			sprintf(errormsg,"RtAudio: Unable to lock DS buffer: %s\n", getErrorMessage(result));
			throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
		}

		//update how many samples still need to be written - possibly no more
		bufsize -= samplesToWrite;
		buf += samplesToWrite;
	} while (bufsize > 0);

	return 0;
}

int RtAudio :: recordBuffer(short *buf, int bufsize)
{
	if (!recording) startRecord();

	if (internalError) {
		throw StkError("RtAudio: Read failure in windoze capture thread.\n",
                   StkError::SOUNDCARD_CONTROL);
	}
  else {
		UINT length; // length of data to copy
		while (bufsize > 0) {
			if (nextRecordWrite > nextRecordRead) {
				length = nextRecordWrite - nextRecordRead;
				if (length > bufsize * sizeof(INT16)) {
          // there may be more data available than our user wants
					length = bufsize * sizeof(INT16);
				}
				CopyMemory(buf, inputBuffer+nextRecordRead, length);
				nextRecordRead += length;
				bufsize -= length / sizeof(INT16);
				buf += length / sizeof(INT16);
			}
      else if (nextRecordWrite == nextRecordRead) {
        // no data is currently available to return to the user -
				// so we sleep for the time necesary to produce the desired
        // number of samples
				Sleep((DWORD) (bufsize / sampleRate * 1000));
			}
      else {
        // this means that our write pointer must have wrapped around -
        // in this case we return the last part ofthe buffer and save the
        // readable beginning of the buffer to be copied next time around
        // the loop, rather than do two copies here - just simpler code
				length = inputBufferSize - nextRecordRead;
				if (length > bufsize * sizeof(INT16)) {
          // there may be more data available than our user wants
					length = bufsize * sizeof(INT16);
					CopyMemory(buf, inputBuffer+nextRecordRead, length);
          // dont fully wrap around - we didn't use all data
					nextRecordRead += length;
				}
        else {
					CopyMemory(buf, inputBuffer+nextRecordRead, length);
					nextRecordRead = 0; // we wrap around
				}
				bufsize -= length / sizeof(INT16);
				buf += length / sizeof(INT16);
			}
		}
	}
	return 0;
}

/**** Windoze specific overload of recordBuffer ****/
int RtAudio :: recordBuffer(INT16** buf)
{
	if (!recording) startRecord();

  if (internalError) {
		throw StkError("RtAudio: Read failure in windoze capture thread.\n",StkError::SOUNDCARD_CONTROL);

	} else if (nextRecordRead == nextRecordWrite) {
    // no new data
		return 0;

	} else {
		BYTE* temp = inputBuffer + nextRecordRead;
		*buf = (INT16*) temp;
		UINT length;

		if (nextRecordWrite > nextRecordRead) {
			length = nextRecordWrite - nextRecordRead;
			nextRecordRead = nextRecordWrite;
		} else {
      /* This means that our write pointer must have wrapped around -
         in this case we return the last part ofthe buffer and save the
         readable begining of the buffer to be returned next call to
         recordBuffer, rather than have some complex two pointer wrap
         around scheme.
      */
			length = inputBufferSize - nextRecordRead;
			nextRecordRead = 0;
		}

    // We divide by two so that we return number of samples, not bytes
		return (length/2);
	}
}

/**** Windows specific start and stop functions ****/
void RtAudio :: stopPlay()
{
	if (!playing) return;

	HRESULT result;
	BYTE* audioPtr;
  DWORD dataLen;

	//stop the buffer
  if (FAILED(result = directSoundBuffer->Stop())) {
		sprintf(errormsg,"RtAudio: Unable to stop DS buffer looping: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
  }

	// Lock the DS buffer and clear it so if we start to play again, we won't have old data playing
	if (FAILED(result = directSoundBuffer->Lock(0, directSoundBufferSize, (LPLPVOID) &audioPtr, &dataLen, NULL, NULL, 0))) {
		sprintf(errormsg,"RtAudio: Unable to lock DS buffer: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
	}

	// Zero the DS buffer
	ZeroMemory(audioPtr, dataLen);

	// Unlock the DS buffer
	if (FAILED(result = directSoundBuffer->Unlock(audioPtr, dataLen, NULL, 0))) {
		sprintf(errormsg,"RtAudio: Unable to unlock DS buffer: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
	}
	//if we start playing again, we must begin at beginning of buffer
	nextWritePos = 0;

	playing = false;
}

void RtAudio :: startPlay()
{
	if (playing) return;

	HRESULT result;
  if (FAILED(result = directSoundBuffer->Play(0, 0, DSBPLAY_LOOPING ))) {
		sprintf(errormsg,"RtAudio: Unable to restart DS buffer: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
  }
	playing = true;
}

void RtAudio :: stopRecord()
{
	if (!recording) return;

	HRESULT result;
	BYTE* audioPtr;
  DWORD dataLen;

	// stop the buffer
  if (FAILED(result = directSoundCaptureBuffer->Stop())) {
		sprintf(errormsg,"RtAudio: Unable to stop DS capture buffer looping: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
  }

	// Lock the DS buffer and clear it so if we start to play again, we won't have old data playing
	if (FAILED(result = directSoundCaptureBuffer->Lock(0, directSoundCaptureBufferSize, (LPLPVOID) &audioPtr, &dataLen, NULL, NULL, 0))) {
		sprintf(errormsg,"RtAudio: Unable to lock DS capture buffer: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
	}

	// Zero the DS buffer
	ZeroMemory(audioPtr, dataLen);

	// Unlock the DS buffer
	if (FAILED(result = directSoundCaptureBuffer->Unlock(audioPtr, dataLen, NULL, 0))) {
		sprintf(errormsg,"RtAudio: Unable to unlock DS capture buffer: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
	}
	// If we start playing again, we must begin at beginning of buffer
	recording = false;
}

void RtAudio :: startRecord()
{
	if (recording) return;

	HRESULT result;
	if (FAILED(result = directSoundCaptureBuffer->Start(DSCBSTART_LOOPING))) {
		sprintf(errormsg,"RtAudio: Unable to restart DS capture buffer looping: %s\n", getErrorMessage(result));
		throw StkError(errormsg,StkError::SOUNDCARD_CONTROL);
  }

	recording = true;
}


/**** Internal Utilities ****/

void RtAudio::getInputSamples()
{
  /* This function is called periodically to get samples from
     the direct sound capture buffer and transfer them into our
     own buffer.
  */
  HRESULT result;
  LPVOID buffer1 = NULL;
  LPVOID buffer2 = NULL;
  DWORD size1 = 0;
  DWORD size2 = 0;
  DWORD capturePos, safePos;
  static UINT nextDirectSoundReadPosition = 0;
  int numBytesToLock;

  // Find out where the write and "safe read" pointers are.
  if (FAILED(result = directSoundCaptureBuffer->GetCurrentPosition(&capturePos, &safePos))) {
		internalError = true;
		return;
	}

	if (!recording) {
    // The user doesn't want us to continue recording, so we must
    // skip over info until they call startRecord().
	  if (safePos < nextDirectSoundReadPosition)
      safePos += directSoundCaptureBufferSize; //unwrap offset
		numBytesToLock = safePos - nextDirectSoundReadPosition; //or in this case, num bytes to skip...
		nextDirectSoundReadPosition = (nextDirectSoundReadPosition + numBytesToLock) % directSoundCaptureBufferSize;
		nextRecordWrite = nextRecordRead;
		return;
	}

	if (safePos == nextDirectSoundReadPosition) {
    // If this case, no input samples are yet ready so we must wait until
    // some are. If this happens alot, we should increase the period of
    // our callback function.
		return;
	}

  if ( safePos < nextDirectSoundReadPosition )
    safePos += directSoundCaptureBufferSize; //unwrap offset
	numBytesToLock = safePos - nextDirectSoundReadPosition;
	UINT availableSpace = inputBufferSize - nextRecordWrite;

	/* This code is rather long and complex, b/c it must handle the data
     transfer from the direct sound capture buffer to our own internal
     buffer, and both, one, or neither buffer may wrap around.
  */
  if (SUCCEEDED(result = directSoundCaptureBuffer->Lock(nextDirectSoundReadPosition, numBytesToLock, &buffer1, &size1, &buffer2, &size2, 0))) {
		if (availableSpace > size1) {
      // we have more than enough space in our internal buffer to hold the data
    	CopyMemory((inputBuffer + nextRecordWrite), buffer1, size1);
    	availableSpace -= size1;
    	nextRecordWrite += size1;

 		} else if (availableSpace == size1) {
      // we have exactly enough space
			CopyMemory((inputBuffer + nextRecordWrite), buffer1, size1);
			availableSpace = inputBufferSize;
    	nextRecordWrite = 0;

		} else {
      // we need to wrap around to the begining of the buffer, ie availableSpace < size1
			CopyMemory((inputBuffer + nextRecordWrite), buffer1, availableSpace);
			size1 -= availableSpace;
			CopyMemory(inputBuffer,(BYTE*)buffer1+availableSpace, size1);
			availableSpace = inputBufferSize - size1;
			nextRecordWrite = size1;
		}

    if (buffer2 != NULL) {
      // this means that the DS buffer wrapped around, and we need to
      // deal with second data portion
			if (availableSpace > size2) {
        // we have more than enough space in our internal buffer to hold data
				CopyMemory((inputBuffer + nextRecordWrite), buffer2, size2);
				nextRecordWrite += size2;

			} else if (availableSpace == size2) {
        // we have exactly enough space
				CopyMemory((inputBuffer + nextRecordWrite), buffer2, size2);
				nextRecordWrite = 0;

			} else {
        // we need to wrap around to the begining of the buffer, ie availableSpace < size2
				CopyMemory((inputBuffer + nextRecordWrite), buffer2, availableSpace);
				size2 -= availableSpace;
				CopyMemory(inputBuffer,(BYTE*)buffer2+availableSpace, size2);
				nextRecordWrite = size2;
			}
		}

		// Update our buffer offset and unlock sound buffer
		directSoundCaptureBuffer->Unlock (buffer1, size1, buffer2, size2);
		nextDirectSoundReadPosition = (nextDirectSoundReadPosition + size1 + size2) % directSoundCaptureBufferSize;
	} else {
    // we were unable to lock the buffer
		internalError = true;
		return;
	}
}

//This static callback function is used by the windows multimedia timer for recording
void CALLBACK RtAudio::PeriodicCallbackFn(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  // dwUser contains ptr to our RtAudio object
  RtAudio * soundIO = (RtAudio *) dwUser;
  (soundIO->getInputSamples)();
}

//Windows forces you to use a callback function to enumerate through sound devices...
bool CALLBACK RtAudio::SoundDeviceEnumCallback(LPGUID lpguid,LPCSTR lpcstrDescription,LPCSTR lpcstrModule, LPVOID lpContext)
//bool CALLBACK RtAudio::SoundDeviceEnumCallback(LPGUID guid, LPCSTR desc, LPCSTR module, LPVOID context)
{
	((RtAudio *) lpContext)->addDevice(lpguid,(char*) lpcstrDescription,(char*) lpcstrModule);

  /*
	if (numDevices < MAX_DEVICES) {
		if (guid != NULL) {
			// make our own copy of the device ID, although probably not necesary
			devices[numDevices].guid = (LPGUID) LocalAlloc(LPTR, sizeof(GUID));
			memcpy(devices[numDevices].guid, guid, sizeof(GUID));
			devices[numDevices].moduleName = module;
			devices[numDevices].description = (char *)desc;
		}
    else {
			devices[numDevices].guid = NULL;
			devices[numDevices].description = "Default Device";
			devices[numDevices].moduleName = "";
		}
		numDevices++;
	}
  */
	return true;
}

void RtAudio::addDevice(LPGUID guid, char* description, char* moduleName)
{
	if (numDevices < MAX_DEVICES) {
		if (guid != NULL) {
			// make our own copy of the device ID, although probably not necesary
			devices[numDevices].guid = (LPGUID) LocalAlloc(LPTR, sizeof(GUID));
			memcpy(devices[numDevices].guid,guid,sizeof(GUID));
			devices[numDevices].moduleName = moduleName;
			devices[numDevices].description = description;
		}
    else {
			devices[numDevices].guid = NULL;
			devices[numDevices].description = "Default Device";
			devices[numDevices].moduleName = "";
		}
		numDevices++;
	}
}

char* RtAudio::getErrorMessage(int code) {
	switch (code) {
  case DSERR_ALLOCATED:
    return "Direct Sound already allocated";

  case DSERR_CONTROLUNAVAIL:
    return "Direct Sound control unavailable";

  case DSERR_INVALIDPARAM:
    return "Direct Sound invalid parameter";

  case DSERR_INVALIDCALL:
    return "Direct Sound invalid call";

  case DSERR_GENERIC:
    return "Direct Sound generic error";

  case DSERR_PRIOLEVELNEEDED:
    return "Direct Sound Priority level needed";

  case DSERR_OUTOFMEMORY:
    return "Direct Sound out of memory";

  case DSERR_BADFORMAT:
    return "Direct Sound bad format";

  case DSERR_UNSUPPORTED:
    return "Direct Sound unsupported error";

  case DSERR_NODRIVER:
    return "Direct Sound no driver error";

  case DSERR_ALREADYINITIALIZED:
    return "Direct Sound already initialized";

  case DSERR_NOAGGREGATION:
    return "Direct Sound no aggregation";

  case DSERR_BUFFERLOST:
    return "Direct Sound buffer lost";

  case DSERR_OTHERAPPHASPRIO:
    return "Direct Sound other app has priority";

  case DSERR_UNINITIALIZED:
    return "Direct Sound uninitialized";

  default:
    return "Direct Sound unknown error";
	}
}

#endif
