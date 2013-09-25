/**********************************************************/
/*  PhISEM (Physically Informed Stochastic Event Modeling */
/*    by Perry R. Cook, Princeton, February 1997          */
/*                                                        */
/*  Meta-model that simulates all of:                     */
/*  Maraca Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Sekere Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Cabasa Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Bamboo Windchime Simulation, by Perry R. Cook, 1996-7 */
/*  Water Drops Simulation, by Perry R. Cook, 1996-7      */
/*  Tambourine Simulation, by Perry R. Cook, 1996-7       */
/*  Sleighbells Simulation, by Perry R. Cook, 1996-7      */
/*  Guiro Simulation, by Perry R. Cook, 1996-7            */
/*                                                        */
/**********************************************************/
/*  PhOLIES (Physically-Oriented Library of               */
/*    Imitated Environmental Sounds), Perry Cook, 1997-9  */
/*  Stix1 (walking on brittle sticks)                     */
/*  Crunch1 (like new fallen snow, or not)                */
/*  Wrench (basic socket wrench, friend of guiro)         */
/*  Sandpapr (sandpaper)                                  */
/**********************************************************/

#if !defined(__Shakers_h)
#define __Shakers_h

#include "Instrmnt.h"

#define MAX_FREQS 8

class Shakers : public Instrmnt
{
 protected:  
  int instType;
  int setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson);
  int ratchetPos, lastRatchetPos;
  MY_FLOAT shakeEnergy;
  MY_FLOAT inputs[MAX_FREQS];
  MY_FLOAT outputs[MAX_FREQS][2];
  MY_FLOAT coeffs[MAX_FREQS][2];
  MY_FLOAT sndLevel;
  MY_FLOAT baseGain;
  MY_FLOAT gains[MAX_FREQS];
  int num_freqs;
  MY_FLOAT t_center_freqs[MAX_FREQS];
  MY_FLOAT center_freqs[MAX_FREQS];
  MY_FLOAT resons[MAX_FREQS];
  MY_FLOAT freq_rand[MAX_FREQS];
  int freqalloc[MAX_FREQS];
  MY_FLOAT soundDecay;
  MY_FLOAT systemDecay;
  MY_FLOAT num_objects;
  MY_FLOAT collLikely;
  MY_FLOAT totalEnergy;
  MY_FLOAT ratchet,ratchetDelta;
  MY_FLOAT finalZ[3];
  MY_FLOAT finalZCoeffs[3];
  void setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay);
  void setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2);
  MY_FLOAT wuter_tick();
  MY_FLOAT ratchet_tick();
 public:
  Shakers();
  ~Shakers();
  int setupName(char* instr);
  int setupNum(int inst);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  MY_FLOAT tick();
  virtual void controlChange(int number, MY_FLOAT value);
};

#endif
