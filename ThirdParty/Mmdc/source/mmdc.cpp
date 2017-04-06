/*
 * The code contained herein is licensed under the MIT License.
 * Copyright (C) 2013-2015 Freescale Semiconductor, Inc. All Rights Reserved.
 */

 /*
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

#ifdef FSL_PLATFORM_YOCTO
/* FIX: For this to be a real library we would need to remove the 'printf' methods or allow the user to specify a logging method. */

#include "mmdc.h"
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{

  const int AXI_BUS_WIDTH_IN_BYTE = 8;

#define MMDC_P0_IPS_BASE_ADDR 0x021B0000
#define MMDC_P1_IPS_BASE_ADDR 0x021B4000

  /************************* Global Variables ***********************************/
  pMMDC_t mmdc_p0 = (pMMDC_t)(MMDC_P0_IPS_BASE_ADDR);
  pMMDC_t mmdc_p1 = (pMMDC_t)(MMDC_P1_IPS_BASE_ADDR);
  /* int g_size = 4; */
  unsigned int system_rev = 0;

  /* modified by the signal handler and main*/
  int g_quit = 0;

  /************************ Profiler Functions **********************************/

  void start_mmdc_profiling(pMMDC_t mmdc)
  {
    /* Reset counters and clear Overflow bit */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x1A;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0xA;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);

    /* Enable counters */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x11;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0x1;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);

  }

  void stop_mmdc_profiling(pMMDC_t mmdc)
  {
    /* Disable counters */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x10;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0x0;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);

  }

  void pause_mmdc_profiling(pMMDC_t mmdc)
  {
    /* PRF_FRZ = 1 */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x13;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0x3;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);
  }

  void resume_mmdc_profiling(pMMDC_t mmdc)
  {
    /* PRF_FRZ = 0 */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x11;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0x1;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);
  }

  void load_mmdc_results(pMMDC_t mmdc)
  {
    /* printf("before : mmdc->madpcr0 0x%x\n",mmdc->madpcr0);*/
    /* sets the PRF_FRZ bit to 1 in order to load the results into the registers */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 |= 0x14;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 |= 0x4;
    else
      return;
    /* printf("after : mmdc->madpcr0 0x%x\n",mmdc->madpcr0); */
    msync(&(mmdc->madpcr0), 4, MS_SYNC);
  }

  void clear_mmdc_results(pMMDC_t mmdc)
  {
    /* Reset counters and clear Overflow bit */
    if (cpu_is_mx6qp() == 1)
      mmdc->madpcr0 = 0x1A;
    else if (cpu_is_mx6q() == 1
      || cpu_is_mx6dl() == 1
      || cpu_is_mx6sl() == 1
      || cpu_is_mx6sx() == 1)
      mmdc->madpcr0 = 0xA;
    else
      return;
    msync(&(mmdc->madpcr0), 4, MS_SYNC);
  }

  void get_mmdc_profiling_results(pMMDC_t mmdc, MMDC_PROFILE_RES_t *results)
  {
    unsigned int bytewidth;
    results->total_cycles = mmdc->madpsr0;
    results->busy_cycles = mmdc->madpsr1;
    results->read_accesses = mmdc->madpsr2;
    results->write_accesses = mmdc->madpsr3;
    results->read_bytes = mmdc->madpsr4;
    results->write_bytes = mmdc->madpsr5;
    bytewidth = 4 << ((mmdc->mdctl & 0x30000) >> 16);
    if (results->read_bytes != 0 || results->write_bytes != 0)
    {
      results->utilization = (int)(((double)results->read_bytes + (double)results->write_bytes) / ((double)results->busy_cycles * bytewidth) * 100);

      results->data_load = (int)((float)results->busy_cycles / (float)results->total_cycles * 100);
      results->access_utilization = (int)(((double)results->read_bytes + (double)results->write_bytes) / ((double)results->read_accesses + (double)results->write_accesses));
      if (mmdc->madpsr3)
        results->avg_write_burstsize = (int)mmdc->madpsr5 / mmdc->madpsr3;
      else
        results->avg_write_burstsize = 0;
      if (mmdc->madpsr2)
        results->avg_read_burstsize = (int)mmdc->madpsr4 / mmdc->madpsr2;
      else
        results->avg_read_burstsize = 0;
    }
    else
    {
      results->utilization = 0;
      results->data_load = 0;
      results->access_utilization = 0;
      results->avg_write_burstsize = 0;
      results->avg_read_burstsize = 0;
    }
  }
  void print_mmdc_profiling_results(MMDC_PROFILE_RES_t results, MMDC_RES_TYPE_t print_type, int time)
  {
    if (print_type == RES_FULL)
    {
      printf("\nMMDC new Profiling results:\n");
      printf("***********************\n");
      printf("Measure time: %dms \n", time);
      printf("Total cycles count: %u\n", results.total_cycles);
      printf("Busy cycles count: %u\n", results.busy_cycles);
      printf("Read accesses count: %u\n", results.read_accesses);
      printf("Write accesses count: %u\n", results.write_accesses);
      printf("Read bytes count: %u\n", results.read_bytes);
      printf("Write bytes count: %u\n", results.write_bytes);
      printf("Avg. Read burst size: %u\n", results.avg_read_burstsize);
      printf("Avg. Write burst size: %u\n", results.avg_write_burstsize);

      printf("Read: ");

      printf("%0.2f MB/s / ", (double)results.read_bytes * 1000 / (1024 * 1024 * time));

      printf(" Write: ");
      printf("%0.2f MB/s ", (double)results.write_bytes * 1000 / (1024 * 1024 * (float)time));

      printf(" Total: ");
      printf("%0.2f MB/s ", (double)(results.write_bytes + results.read_bytes) * 1000 / (1024 * 1024 * (float)time));

      printf("\r\n");
    }
    if ((results.utilization > 100) || (results.data_load > 100))
    {
      printf("Warning:Counter overflow!!!Record time too long!!!\n");
    }
    else	{
      printf("Utilization: %u%%\n", results.utilization);
      printf("Overall Bus Load: %u%%\n", results.data_load);
      printf("Bytes Access: %u\n\n", results.access_utilization);
    }
  }

  static int get_system_rev(void)
  {
    FILE *fp;
    char buf[2048];
    int nread;
    char *tmp, *rev;
    int rev_major, rev_minor;
    int ret = -1;

    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL) {
      perror("/proc/cpuinfo\n");
      return ret;
    }

    nread = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);
    if ((nread == 0) || (nread == sizeof(buf))) {
      printf("/proc/cpuinfo: %d\n", nread);
      nread = 0;
      ret = 0;
    }

    buf[nread] = '\0';

    tmp = strstr(buf, "Revision");
    if (tmp != NULL) {
      rev = index(tmp, ':');
      if (rev != NULL) {
        rev++;
        system_rev = strtoul(rev, NULL, 16);
        ret = 0;
      }
    }

    /* cpuinfo is changed in 3.10.9 kernel, new way is used. */
    if ((ret == 0) && (system_rev == 0))
    {
      ret = -1;
      fp = fopen("/sys/devices/soc0/soc_id", "r");
      if (fp == NULL) {
        perror("/sys/devices/soc0/soc_id\n");
        return ret;
      }

      nread = fread(buf, 1, sizeof(buf), fp);
      fclose(fp);
      if ((nread == 0) || (nread == sizeof(buf))) {
        printf("/sys/devices/soc0/soc_id:%d\n", nread);
        return ret;
      }
      buf[nread] = '\0';
      fp = fopen("/sys/devices/soc0/revision", "r");
      if (fp == NULL) {
        perror("/sys/devices/soc0/revision");
        return ret;
      }

      if (fscanf(fp, "%d.%d", &rev_major, &rev_minor) != 2) {
        perror("fscanf");
        fclose(fp);
        return ret;
      }
      fclose(fp);

      if (strncmp(buf, "i.MX6Q", 6) == 0)
      {
        if (rev_major == CHIP_REV_2_0)
        {
          system_rev = 0x65000;
          ret = 0;
          printf("i.MX6QP detected.\n");
        }
        else if (rev_major == CHIP_REV_1_0)
        {
          system_rev = 0x63000;
          ret = 0;
          printf("i.MX6Q detected.\n");
        }
      }
      else if (strncmp(buf, "i.MX6DL", 7) == 0){
        system_rev = 0x61000;
        ret = 0;
        printf("i.MX6DL detected.\n");
      }
      else if (strncmp(buf, "i.MX6SL", 7) == 0)
      {
        system_rev = 0x60000;
        ret = 0;
        printf("i.MX6SL detected.\n");
      }
      else if (strncmp(buf, "i.MX6SX", 7) == 0)
      {
        system_rev = 0x62000;
        ret = 0;
        printf("i.MX6SX detected.\n");
      }
    }
    return ret;
  }


  int get_mmdc_info(MMDC_INFO_t* pInfo)
  {
    char *p;

    if (pInfo == NULL)
    {
      printf("info can not be a NULL pointer");
      return 0;
    }

    /* Set default configuration */
    pInfo->time_for_sleep_milliseconds = 500;
    pInfo->loop_count = 1;

    p = getenv("MMDC_SLEEPTIME");
    if (p != 0)
    {
      pInfo->time_for_sleep_milliseconds = strtol(p, 0, 10);
      if (pInfo->time_for_sleep_milliseconds == 0)
        pInfo->time_for_sleep_milliseconds = 500;
    }
    p = getenv("MMDC_LOOPCOUNT");
    if (p != 0)
    {
      pInfo->loop_count = strtol(p, 0, 10);
    }
    return 1;
  }


  int begin_mmdc(MMDC_CONTEXT_t* pContext)
  {
    void* A;
    unsigned int customized_madpcr1 = 0;
    char* pszValue;
    int fd;

    if (pContext == NULL)
    {
      printf("mmdc context can not be a NULL pointer");
      return 0;
    }

    pContext->mmdc = 0;
    pContext->fd = -1;
    pContext->system_rev_status = -1;
    pContext->customized_madpcr1 = customized_madpcr1;

    pszValue = getenv("MMDC_CUST_MADPCR1");
    if (pszValue != 0)
    {
      customized_madpcr1 = strtol(pszValue, 0, 16);
    }

    fd = open("/dev/mem", O_RDWR, 0);
    if (fd < 0)
    {
      printf("Could not open /dev/mem\n");
      return 0;
    }

    A = (unsigned long*)mmap(NULL, 0x4000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMDC_P0_IPS_BASE_ADDR);
    if (A == MAP_FAILED)
    {
      printf("Mapping failed mmdc_p0\n");
      close(fd);
      return 0;
    }

    /* Fill the context with the valid data */
    pContext->mmdc = (pMMDC_t)A;
    pContext->fd = fd;
    pContext->system_rev_status = get_system_rev();
    pContext->customized_madpcr1 = customized_madpcr1;
    return 1;
  }

  void end_mmdc(MMDC_CONTEXT_t* pContext)
  {
    if (pContext == NULL)
      return;

    if (pContext->mmdc != NULL)
    {
      munmap(pContext->mmdc, 0x4000);
      pContext->mmdc = 0;
    }

    if (pContext->fd >= 0)
    {
      close(pContext->fd);
      pContext->fd = 0;
    }
  }


  /* FIX: using a string for the type is ugly. We ought to use a enum instead.
          We could still have a helper method that translates from a string to the enum. */
  int auto_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, char* pszType, int enablePrint)
  {
    unsigned int type = axi_default;

    if (pContext == NULL || pContext->mmdc == NULL)
    {
      printf("Invalid mmdc context");
      return 0;
    }

    /* If type is a nullptr use the default configuration, if not we try to detect what the user requested */
    if (pszType == NULL)
    {
      if (pContext->customized_madpcr1 != 0)
      {
        manual_configuration_mmdc_profiling(pContext, pContext->customized_madpcr1);
        if (enablePrint)
          printf("MMDC 0x%x \n", pContext->customized_madpcr1);
      }
      else
      {
        manual_configuration_mmdc_profiling(pContext, axi_default);
        if (enablePrint)
          printf("MMDC SUM \n");
      }
      return 1;
    }

    if (strcmp(pszType, "DSP1") == 0){
      if (cpu_is_mx6sx() == 1)
        type = axi_lcd1_6sx;
      else if (cpu_is_mx6sl() == 1)
        type = axi_lcd1_6sl;
      else
        type = axi_ipu1;
      if (enablePrint)
        printf("MMDC DSP1 \n");
    }
    else if ((strcmp(pszType, "DSP2") == 0) && (cpu_is_mx6q() == 1)){
      type = axi_ipu2_6q;
      if (enablePrint)
        printf("MMDC DSP2 \n");
    }
    else if ((strcmp(pszType, "DSP2") == 0) && (cpu_is_mx6sx() == 1)){
      type = axi_lcd2_6sx;
      if (enablePrint)
        printf("MMDC DSP2 \n");
    }
    else if ((strcmp(pszType, "M4") == 0) && (cpu_is_mx6sx() == 1)){
      type = axi_m4_6sx;
      if (enablePrint)
        printf("MMDC M4 \n");
    }
    else if ((strcmp(pszType, "PXP") == 0) && (cpu_is_mx6sx() == 1)){
      type = axi_pxp_6sx;
      if (enablePrint)
        printf("MMDC M4 \n");
    }
    else if ((strcmp(pszType, "GPU3D") == 0) && (cpu_is_mx6sx() == 1)){
      type = axi_gpu3d_6sx;
      if (enablePrint)
        printf("MMDC GPU3D \n");
    }
    else if ((strcmp(pszType, "GPU3D") == 0) && (cpu_is_mx6dl() == 1)){
      type = axi_gpu3d_6dl;
      if (enablePrint)
        printf("MMDC GPU3D \n");
    }
    else if ((strcmp(pszType, "GPU3D") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_gpu3d_6qp;
      if (enablePrint)
        printf("MMDC GPU3D \n");
    }
    else if ((strcmp(pszType, "GPU3D") == 0) && (cpu_is_mx6q() == 1)){
      type = axi_gpu3d_6q;
      if (enablePrint)
        printf("MMDC GPU3D \n");
    }
    else if ((strcmp(pszType, "GPU2D1") == 0) && (cpu_is_mx6dl() == 1)){
      type = axi_gpu2d1_6dl;
      if (enablePrint)
        printf("MMDC GPU2D1 \n");
    }
    else if ((strcmp(pszType, "GPU2D") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_gpu2d_6qp;
      if (enablePrint)
        printf("MMDC GPU2D \n");
    }
    else if ((strcmp(pszType, "GPU2D") == 0) && (cpu_is_mx6q() == 1)){
      type = axi_gpu2d_6q;
      if (enablePrint)
        printf("MMDC GPU2D \n");
    }
    else if ((strcmp(pszType, "GPU2D2") == 0) && (cpu_is_mx6dl() == 1)){
      type = axi_gpu2d2_6dl;
      if (enablePrint)
        printf("MMDC GPU2D2 \n");
    }
    else if ((strcmp(pszType, "GPU2D") == 0) && (cpu_is_mx6sl() == 1)){
      type = axi_gpu2d_6sl;
      if (enablePrint)
        printf("MMDC GPU2D \n");
    }
    else if ((strcmp(pszType, "VPU") == 0) && (cpu_is_mx6dl() == 1)){
      type = axi_vpu_6dl;
      if (enablePrint)
        printf("MMDC VPU \n");
    }
    else if ((strcmp(pszType, "VPU") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_vpu_6qp;
      if (enablePrint)
        printf("MMDC VPU \n");
    }
    else if ((strcmp(pszType, "VPU") == 0) && (cpu_is_mx6q() == 1)){
      type = axi_vpu_6q;
      if (enablePrint)
        printf("MMDC VPU \n");
    }
    else if ((strcmp(pszType, "PRE") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_pre_6qp;
      if (enablePrint)
        printf("MMDC PRE \n");
    }
    else if ((strcmp(pszType, "PRE0") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_pre0_6qp;
      if (enablePrint)
        printf("MMDC PRE0 \n");
    }
    else if ((strcmp(pszType, "PRE1") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_pre1_6qp;
      if (enablePrint)
        printf("MMDC PRE1 \n");
    }
    else if ((strcmp(pszType, "PRE2") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_pre2_6qp;
      if (enablePrint)
        printf("MMDC PRE2 \n");
    }
    else if ((strcmp(pszType, "PRE3") == 0) && (cpu_is_mx6qp() == 1)){
      type = axi_pre3_6qp;
      if (enablePrint)
        printf("MMDC PRE3 \n");
    }
    else if ((strcmp(pszType, "GPUVG") == 0) && (cpu_is_mx6q() == 1)){
      type = axi_openvg_6q;
      if (enablePrint)
        printf("MMDC GPUVG \n");
    }
    else if ((strcmp(pszType, "GPUVG") == 0) && (cpu_is_mx6sl() == 1)){
      type = axi_openvg_6sl;
      if (enablePrint)
        printf("MMDC GPUVG \n");
    }
    else if (strcmp(pszType, "USB") == 0){
      if (cpu_is_mx6sx() == 1)
        type = axi_usb_6sx;
      else if (cpu_is_mx6sl() == 1)
        type = axi_usb_6sl;
      else
        type = axi_usb;
      if (enablePrint)
        printf("MMDC USB \n");
    }
    else if (strcmp(pszType, "ARM") == 0){
      if ((cpu_is_mx6sx() == 1))
        type = axi_arm_6sx;
      else
        type = axi_arm;
      if (enablePrint)
        printf("MMDC ARM \n");
    }
    else if (strcmp(pszType, "SUM") == 0){
      type = axi_default;
      if (enablePrint)
        printf("MMDC SUM \n");
    }
    else{
      printf("MMDC DOES NOT KNOW %s \n", pszType);
      return 0;
    }
    manual_configuration_mmdc_profiling(pContext, type);
    return 1;
  }


  void manual_configuration_mmdc_profiling(MMDC_CONTEXT_t* pContext, unsigned int value)
  {
    if (pContext == NULL || pContext->mmdc == NULL)
    {
      printf("Invalid mmdc context");
      return;
    }

    pContext->mmdc->madpcr1 = value;
    msync(&(pContext->mmdc->madpcr1), 4, MS_SYNC);
  }


  /* FIX: The code inside this ifdef really ought to be a in separate file */
#ifndef FSL_MMDC_AS_LIBRARY

  static unsigned long getTickCount(void)
  {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0)
      return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
  }


  void signalhandler(int signal)
  {
    /* FIX: It's not safe to call printf in a signal handler, but this code appears to be unused, I suggest we remove this printf entirely (or do it in main) */
    printf("signal %d received, quit the application.\n", signal);
    g_quit = 1;
  }

  void help(void)
  {
    printf("======================MMDC v1.3===========================\n");
    printf("Usage: mmdc [ARM:DSP1:DSP2:GPU2D:GPU2D1:GPU2D2:GPU3D:GPUVG:VPU:M4:PXP:USB:SUM] [...]\n");
    printf("export MMDC_SLEEPTIME can be used to define profiling duration, 500 by default means 0.5s\n");
    printf("export MMDC_LOOPCOUNT can be used to define profiling times. 1 by default. -1 means infinite loop.\n");
    printf("export MMDC_CUST_MADPCR1 can be used to customize madpcr1. Will ignore it if defined master\n");
    printf("Note1: More than 1 master can be inputed. They will be profiled one by one.\n");
    printf("Note2: MX6DL can't profile master GPU2D, GPU2D1 and GPU2D2 are used instead.\n");
  }


  static void ProfileNow(MMDC_CONTEXT_t* pContext, int timeForSleepMilliseconds)
  {
    int ulStartTime = 0;

    if (pContext == NULL || pContext->mmdc == NULL)
    {
      printf("Invalid mmdc context");
      return;
    }
    if (pInfo == NULL)
    {
      printf("Invalid mmdc info");
      return;
    }

    clear_mmdc_results(pContext->mmdc);
    ulStartTime = getTickCount();
    start_mmdc_profiling(pContext->mmdc);
    usleep(timeForSleepMilliseconds * 1000);
    load_mmdc_results(pContext->mmdc);
    get_mmdc_profiling_results(pContext->mmdc, &results);
    print_mmdc_profiling_results(results, RES_FULL, getTickCount() - ulStartTime);
    fflush(stdout);
    stop_mmdc_profiling(pContext->mmdc);
  }


  int main(int argc, char **argv)
  {
    int i;
    MMDC_INFO_t info;
    MMDC_CONTEXT_t context;
    MMDC_PROFILE_RES_t results;

    /* Retrieve the default configuration information */
    if (get_mmdc_info(&info) == 0)
    {
      printf("Failed to get mmdc info");
      return -1;
    }

    /* Prepare the mmdc context */
    if (begin_mmdc(&context) == 0)
      return -1;

    if (context.system_rev_status < 0)
    {
      printf("Fail to get system revision, parameter will be ignored \n");
      argc = 1;
    }

    g_quit = 0;
    for (i = 0; !g_quit && i != info.loop_count; i++)
    {
      if (argc >= 2)
      {
        int j;
        for (j = 1; j < argc; j++)
        {
          if (auto_configuration_mmdc_profiling(&context, argv[j]) < 0, 1)
          {
            help();
            end_mmdc(&context);
            return 0;
          }

          ProfileNow(&context, info.time_for_sleep_milliseconds);
        }
      }
      else
      {
        if (auto_configuration_mmdc_profiling(&context, NULL) < 0, 1)
        {
          help();
          end_mmdc(&context);
          return 0;
        }

        ProfileNow(&context, info.time_for_sleep_milliseconds);
      }
    }
    end_mmdc(&context);
    return 0;
  }
#endif

}
#endif
