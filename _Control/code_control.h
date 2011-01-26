#ifndef CODE_CONTROL_H
#define CODE_CONTROL_H

#include <iostream>
#include <iomanip>

#include "ATbase.h"

#include "control.h"



/**
 *  \ingroup control_grp 
 *
 *  \class CodeControl
 *
 *  \brief Controls switching on/off of different blocks of the code
 * 
 *  All parameters are optional and <b> if they are not set
 *  they are true by default </b>
 *
 * config file example
 \verbatim

     Group "CODE_CONTROL" {
     
	! all optional <<<<<<<<<

	! Electrodynamics -------------
        DoGatherRhoAndJ ? Yes;
        DoMaxwell       ? Yes;

        ! Enforce Gauss Law -----------
        DoEnforceGaussLaw ? Yes;
        NSteps_EnforceGaussLaw = 50;

	! Particles -------------------
        DoMoveParticles ? Yes;

	! Monte Carlo -----------------
        DoMonteCarlo    ? Yes;
        DoCreatePairs   ? Yes;
        DoInjectPairs   ? Yes;

	! LPT -------------------------
        DoLPT                 ? Yes;
	DoLPTforTimeLaterThan = 1e-3;
     }     

 \endverbatim
 *
 */
class CodeControl: public Control
{
public:

  //! Collect charge and current densities?
  bool DoGatherRhoAndJ() const;

  //! Evolve Electromagnetic field?
  bool DoMaxwell() const;

  //! Enforce Gauss Law?
  bool DoEnforceGaussLaw() const;
  //! Enforce Gauss Law at iteration# it?
  bool DoEnforceGaussLaw(int it) const;

  //! Do "Gather" and "Maxwell" parts?
  bool DoMoveParticles() const;

  //! do ALL "MonteCarlo" parts?
  bool DoMonteCarlo() const;
  //! do create pairs
  bool DoCreatePairs() const;
  //! do inject pairs
  bool DoInjectPairs() const;

  //! Do Particle Number adjustment (no args)
  bool DoLPT() const; 
  //! Do Particle Number adjustment
  bool DoLPT(double t) const; 


  //! Setup data from config file
  void SetupFromConfig(FileInput &in);

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;


private:

  static bool _DoGatherRhoAndJ;
  static bool _DoMaxwell;

  static bool _DoEnforceGaussLaw;
  static int  _NSteps_EnforceGaussLaw;

  static bool _DoMoveParticles;

  static bool _DoMonteCarlo;
  static bool _DoCreatePairs;
  static bool _DoInjectPairs;

  static bool   _DoLPT;
  static double _T_LPT;
};


//! Collect charge and current densities?
inline bool CodeControl::DoGatherRhoAndJ() const 
{
#ifdef TEST_CODE
  std::cout<<"->DoGatherRhoAndJ\n"<<std::flush;
#endif
 
  return _DoGatherRhoAndJ; 
};

//! Evolve Electromagnetic field?
inline bool CodeControl::DoMaxwell() const   
{ 
#ifdef TEST_CODE
  std::cout<<"->DoMaxwell\n"<<std::flush;
#endif

  return _DoMaxwell; 
};


//! Enforse Gauss Law?
inline bool CodeControl::DoEnforceGaussLaw() const
{
  return _DoEnforceGaussLaw;
}

//! Enforse Gauss Law at step <b>it</b>?
inline bool CodeControl::DoEnforceGaussLaw(int it) const
{
  return _DoEnforceGaussLaw && ( it % _NSteps_EnforceGaussLaw == 0 );
}


//! Do "Gather" and "Maxwell" parts?
inline bool CodeControl::DoMoveParticles() const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoMoveParticles\n"<<std::flush;
#endif

return _DoMoveParticles; 
};


//! do all "MonteCarlo" parts?
inline bool CodeControl::DoMonteCarlo() const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoMonteCarlo\n"<<std::flush;
#endif

return _DoMonteCarlo; 
};


//! do all "MonteCarlo" parts?
inline bool CodeControl::DoCreatePairs() const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoCreatePairs\n"<<std::flush;
#endif

return _DoMonteCarlo && _DoCreatePairs; 
};


//! do all "MonteCarlo" parts?
inline bool CodeControl::DoInjectPairs() const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoInjectPairs\n"<<std::flush;
#endif

return _DoMonteCarlo && _DoInjectPairs; 
};




/** 
 * Do Particle Number adjustment?
 */
inline bool CodeControl::DoLPT() const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoLPT()\n"<<std::flush;
#endif

return _DoLPT; 
};

/** 
 * Do Particle Number adjustment? (if  t>=_T_LPT)
 * 
 * @param t current time
 */
inline bool CodeControl::DoLPT(double t) const 
{ 
#ifdef TEST_CODE
  std::cout<<"->DoLPT(t)\n"<<std::flush;
#endif

return _DoLPT && t>=_T_LPT; 
};


#endif
