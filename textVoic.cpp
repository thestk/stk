/*  Very simple text input program for formant voice
	model.  Perry R. Cook    1996             */

/************   Test Main Program   *****************/

#include "VoicForm.h"
#include "RawWvOut.h"
#include "Reverb.h"

int main(int argc, char *argv[])
{
    VoicForm instrument;
    RawWvOut output("test.snd");
    Reverb reverb(2137);
    long i,j,k;
    double temp = 110.0;

    if (argc>1) {
    
      instrument.setPitchSweepRate(0.001);
      instrument.setFreq(temp);
      reverb.setEffectMix(0.2);
      for (j=1;j<argc;j++)        {
        if (argv[j][0] == '+')  {
            i = 0;
            while (argv[j][i] == '+') {
                temp *= 1.059;
                i++;
            }
            instrument.setFreq(temp);
            printf("New Freq: %lf\n",temp);
        }
        else if (argv[j][0] == '-')  {
            i = 0;
            while (argv[j][i] == '-') {
                temp /= 1.059;
                i++;
            }
            instrument.setFreq(temp);
            printf("New Freq: %lf\n",temp);
        }
        else if (!strcmp("_",argv[j]))  {
          instrument.quiet();
          printf("Quieting \n");
          for (i=0;i<1378;i++) {
              output.tick(reverb.tick(instrument.tick()));
          }
        }    
        else if (argv[j][0] == '=')  {
          temp = atof(&argv[j][1]);
          instrument.setFreq(temp);
          printf("Setting absolute freq to : %f \n",temp);
        }    
        else if (argv[j][0] == '.')  {
            i = 0;
            while (argv[j][i] == '.') {
                i++;
            }
            k = i * 2756;
            for (i=0;i<k;i++) {
              output.tick(reverb.tick(instrument.tick()));
            }
        }
        else if (instrument.setPhoneme(argv[j]))     { 
            printf("Phoneme set\n");
            for (i=0;i<2756;i++) {
              output.tick(reverb.tick(instrument.tick()));
            }
        }
      }
      instrument.quiet();
      printf("Quieting \n");
      for (i=0;i<22050;i++) {
              output.tick(reverb.tick(0.0));
      }
      printf("All Done!!!\n");
    }
    else printf("Useage: voicform phon phon ...\n");
}

