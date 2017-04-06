#ifndef FSL_PLATFORM_YOCTO
#include <mmdc.h>

extern "C"
{

  int get_mmdc_info(MMDC_INFO_t* pInfo)
  {
    return 0;
  }


  int begin_mmdc(MMDC_CONTEXT_t* pContext)
  {
    return 0;
  }

  void end_mmdc(MMDC_CONTEXT_t* pContext)
  {
  }


  int auto_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, char* pszType, int enablePrint)
  {
    return 0;
  }


  void manual_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, unsigned int value)
  {

  }


  void start_mmdc_profiling(pMMDC_t mmdc)
  {
  }


  void stop_mmdc_profiling(pMMDC_t mmdc)
  {
  }


  void pause_mmdc_profiling(pMMDC_t mmdc)
  {
  }


  void resume_mmdc_profiling(pMMDC_t mmdc)
  {
  }


  void get_mmdc_profiling_results(pMMDC_t mmdc, MMDC_PROFILE_RES_t *results)
  {
  }


  void print_mmdc_profiling_results(MMDC_PROFILE_RES_t results, MMDC_RES_TYPE_t print_type,int time)
  {
  }


  void clear_mmdc_results(pMMDC_t mmdc)
  {
  }


  void load_mmdc_results(pMMDC_t mmdc)
  {
  }
}

#endif
