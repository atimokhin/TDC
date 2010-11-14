#include "code_control.h"


bool CodeControl::_DoGatherRhoAndJ = true;
bool CodeControl::_DoMaxwell       = true;
bool CodeControl::_DoMoveParticles = true;

bool CodeControl::_DoMonteCarlo    = true;
bool CodeControl::_DoCreatePairs   = true;
bool CodeControl::_DoInjectPairs   = true;

bool CodeControl::_DoLPT = true;
double CodeControl::_T_LPT = 0;


/** 
 * Reads data from the input file and sets cascade test parameters
 * 
 */
void CodeControl::SetupFromConfig(FileInput &in)
{
  ATbase::Group_t *p_current_group = in.Get_pCurrentGroup();
  in.ChangeGroup();

  // Read parameters from "TEST_CASCADE" group
  in.ChangeGroup("CODE_CONTROL");

  if ( in.answer_is_set("DoGatherRhoAndJ"))
    _DoGatherRhoAndJ = in.get_answer("DoGatherRhoAndJ");

  if ( in.answer_is_set("DoMaxwell"))
    _DoMaxwell = in.get_answer("DoMaxwell");

  if ( in.answer_is_set("DoMoveParticles"))
    _DoMoveParticles = in.get_answer("DoMoveParticles");


  if ( in.answer_is_set("DoMonteCarlo"))
    _DoMonteCarlo = in.get_answer("DoMonteCarlo");
  if ( in.answer_is_set("DoCreatePairs"))
    _DoCreatePairs = in.get_answer("DoCreatePairs");
  if ( in.answer_is_set("DoInjectPairs"))
    _DoInjectPairs = in.get_answer("DoInjectPairs");


  if ( in.answer_is_set("DoLPT") )
    _DoLPT = in.get_answer("DoLPT");
  if ( in.param_is_set("DoLPTforTimeLaterThan") )
    _T_LPT = in.get_param("DoLPTforTimeLaterThan");

  in.ChangeGroup(p_current_group);
}


/** 
 * Print Test Cascade parameters
 * 
 */
std::ostream& CodeControl::Print(std::ostream& s) const
{
  s<<"\n Code control:\n\n";
  s<<std::boolalpha;
  s<<" Gather charge and current densities ? "<<DoGatherRhoAndJ()<<"\n";
  s<<"             Solve Maxwell Equations ? "<<DoMaxwell()<<"\n";
  s<<"                      Move Particles ? "<<DoMoveParticles()<<"\n";
  s<<"                      Do Monte-Carlo ? "<<DoMonteCarlo()<<"\n";
  s<<"                     Do Create Pairs ? "<<DoCreatePairs()<<"\n";
  s<<"                     Do Inject Pairs ? "<<DoInjectPairs()<<"\n";
  s<<"\n";
  s<<std::resetiosflags(std::ios_base::boolalpha)<<std::flush;

  return s;
}
