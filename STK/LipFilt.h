/************************************************/
/*  Lip Filter Object by Perry R. Cook, 1995-96 */
/*  The lip of the brass player has dynamics    */
/*  which are controlled by the mass, spring    */
/*  constant, and damping of the lip.  This     */
/*  filter simulates that behavior and the      */
/*  transmission/reflection properties as       */
/*  well.  See Cook TBone and HosePlayer        */
/*  instruments and articles.                   */
/************************************************/

#include "Object.h"
#include "BiQuad.h"

class LipFilt : public Object
{
protected:
  BiQuad *filter;
  MY_FLOAT lastOutput;
public:
  LipFilt();
  ~LipFilt();
  void clear();
  void setFreq(MY_FLOAT frequency);
  MY_FLOAT tick(MY_FLOAT mouthSample,MY_FLOAT boreSample);
  MY_FLOAT lastOut();
};

