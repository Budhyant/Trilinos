#ifndef __Panzer_LOCPair_GlobalEvaluationData_hpp__
#define __Panzer_LOCPair_GlobalEvaluationData_hpp__

#include "Panzer_config.hpp"
#include "Panzer_Traits.hpp"
#include "Panzer_LinearObjFactory.hpp"

namespace panzer {

/** Class that overides the communication primitives
  * to do nothing. This is used by the <code>LinearObjContainer</code>.
  */
class LOCPair_GlobalEvaluationData : public GlobalEvaluationData {
public:
   LOCPair_GlobalEvaluationData(
                 Teuchos::RCP<const LinearObjFactory<panzer::Traits> > lof,
                 int initParam) : lof_(lof), initParam_(initParam) 
   {
      globalLOC_ = lof_->buildLinearObjContainer();
      ghostedLOC_ = lof_->buildGhostedLinearObjContainer();

      lof_->initializeContainer(initParam,*globalLOC_);
      lof_->initializeGhostedContainer(initParam,*ghostedLOC_);
   }
  
   virtual void ghostToGlobal(int mem) { lof_->ghostToGlobalContainer(*ghostedLOC_,*globalLOC_,mem); }
   virtual void globalToGhost(int mem) { lof_->globalToGhostContainer(*globalLOC_,*ghostedLOC_,mem); }

   virtual void initializeData() { ghostedLOC_->initialize(); } 

   Teuchos::RCP<LinearObjContainer> getGhostedLOC() const { return ghostedLOC_; }
   Teuchos::RCP<LinearObjContainer> getGlobalLOC() const { return globalLOC_; }

private:
   Teuchos::RCP<LinearObjContainer> ghostedLOC_, globalLOC_;

   Teuchos::RCP<const LinearObjFactory<panzer::Traits> > lof_; 
   int initParam_;
};

}

#endif
