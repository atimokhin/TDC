#ifndef NS_CONSTS_H
#define NS_CONSTS_H

#include <iostream>
#include <string>

#include "ATbase.h"

/**
 * \ingroup setup_consts_grp
 *
 * \class  NSConsts
 * \brief  Contains constants related to the neutron star properties
 *
 * Contains constants related to the neutron star properties
 * - \f$ R_{NS,6} \f$   --  radius of the neutron star
 * 
 * <b> by default  RNS_6=1 but it can be set to a different value in config file </b>
 *
 * configuration file example:
 \verbatim

   Group "DIMENSIONAL_CONSTANTS" {
      InitializerType='Some_ConstsInitializer';
   
      Group "Some_ConstsInitializer" {
         ...
         ! Optional:
         ! NS radius [10^6 cm]
         RNS_6 = 1;
      }

 \endverbatim
 */
class NSConsts
{
public:
  //! Setup from config file group
  void SetupFromConfigGroup(FileInput &in);

  //! neutron star radius 
  double RNS_6() const { return _RNS_6; };

  //! Print physical paramneters
  std::ostream& Print(std::ostream& s) const;

protected:
  static double _RNS_6;
};


inline std::ostream& operator<< (std::ostream& s, const NSConsts& ns)
{
  return ns.Print(s);
};
#endif
