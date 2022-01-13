/* Automatically generated file; DO NOT EDIT */
/* Espressif IoT Development Framework Linker Script */
/* Generated from: /home/ros/esp/esp-idf/components/esp32/ld/esp32.project.ld.in */

/*  Default entry point:  */
ENTRY(call_start_cpu0);

SECTIONS
{
  /* RTC fast memory holds RTC wake stub code,
     including from any source file named rtc_wake_stub*.c
  */
  .rtc.text :
  {
    . = ALIGN(4);

    *( .rtc.literal  .rtc.text EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .rtc.text.*)
    *libfreertos.a:port.*( .rtc.text.*)
    *libfreertos.a:queue.*( .rtc.text.*)
    *libfreertos.a:tasks.*( .rtc.text.*)
    *libfreertos.a:port.*(.rtc.text.pxPortInitialiseStack)
    *libfreertos.a:port.*(.rtc.text.xPortStartScheduler)
    *libfreertos.a:queue.*(.rtc.text.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.rtc.text.vTaskStartScheduler)

    *rtc_wake_stub*.*(.literal .text .literal.* .text.*)
    _rtc_text_end = ABSOLUTE(.);
  } > rtc_iram_seg

  /*
    This section is required to skip rtc.text area because rtc_iram_seg and
    rtc_data_seg are reflect the same address space on different buses.
  */
  .rtc.dummy :
  {
    _rtc_dummy_start = ABSOLUTE(.);
    _rtc_fast_start = ABSOLUTE(.);
    . = SIZEOF(.rtc.text);
    _rtc_dummy_end = ABSOLUTE(.);
  } > rtc_data_seg

  /* This section located in RTC FAST Memory area.
     It holds data marked with RTC_FAST_ATTR attribute.
     See the file "esp_attr.h" for more information.
  */
  .rtc.force_fast :
  {
    . = ALIGN(4);
    _rtc_force_fast_start = ABSOLUTE(.);
    *(.rtc.force_fast .rtc.force_fast.*)
    . = ALIGN(4) ;
    _rtc_force_fast_end = ABSOLUTE(.);
  } > rtc_data_seg

  /* RTC data section holds RTC wake stub
     data/rodata, including from any source file
     named rtc_wake_stub*.c and the data marked with
     RTC_DATA_ATTR, RTC_RODATA_ATTR attributes.
     The memory location of the data is dependent on
     CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
  */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);

    *( .rtc.data EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .rtc.data.*  .rtc.rodata EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .rtc.rodata.*)
    *libfreertos.a:port.*( .rtc.data.*  .rtc.rodata.*)
    *libfreertos.a:queue.*( .rtc.data.*  .rtc.rodata.*)
    *libfreertos.a:tasks.*( .rtc.data.*  .rtc.rodata.*)
    *libfreertos.a:port.*(.rtc.data.pxPortInitialiseStack .rtc.rodata.pxPortInitialiseStack)
    *libfreertos.a:port.*(.rtc.data.xPortStartScheduler .rtc.rodata.xPortStartScheduler)
    *libfreertos.a:queue.*(.rtc.data.xQueueGenericCreateStatic .rtc.rodata.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.rtc.data.vTaskStartScheduler .rtc.rodata.vTaskStartScheduler)

    *rtc_wake_stub*.*(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_data_location

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.*(.bss .bss.*)
    *rtc_wake_stub*.*(COMMON)

    *( .rtc.bss)

    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_data_location

  /* This section holds data that should not be initialized at power up
     and will be retained during deep sleep.
     User data marked with RTC_NOINIT_ATTR will be placed
     into this section. See the file "esp_attr.h" for more information.
	 The memory location of the data is dependent on
     CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
  */
  .rtc_noinit (NOLOAD):
  {
    . = ALIGN(4);
    _rtc_noinit_start = ABSOLUTE(.);
    *(.rtc_noinit .rtc_noinit.*)
    . = ALIGN(4) ;
    _rtc_noinit_end = ABSOLUTE(.);
  } > rtc_data_location

  /* This section located in RTC SLOW Memory area.
     It holds data marked with RTC_SLOW_ATTR attribute.
     See the file "esp_attr.h" for more information.
  */
  .rtc.force_slow :
  {
    . = ALIGN(4);
    _rtc_force_slow_start = ABSOLUTE(.);
    *(.rtc.force_slow .rtc.force_slow.*)
    . = ALIGN(4) ;
    _rtc_force_slow_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* Get size of rtc slow data based on rtc_data_location alias */
  _rtc_slow_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_slow_end - _rtc_data_start)
                        : (_rtc_force_slow_end - _rtc_force_slow_start);

  _rtc_fast_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_fast_end - _rtc_fast_start)
                        : (_rtc_noinit_end - _rtc_fast_start);

  ASSERT((_rtc_slow_length <= LENGTH(rtc_slow_seg)),
          "RTC_SLOW segment data does not fit.")

  ASSERT((_rtc_fast_length <= LENGTH(rtc_data_seg)),
          "RTC_FAST segment data does not fit.")

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    _iram_start = ABSOLUTE(.);
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);

    *( .iram1 EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .iram1.*)
    *libapp_trace.a:app_trace.*( .literal  .literal.*  .text  .text.*)
    *libapp_trace.a:app_trace_util.*( .literal  .literal.*  .text  .text.*)
    *libc.a:creat.*( .literal  .literal.*  .text  .text.*)
    *libc.a:isatty.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-abs.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-asctime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-asctime_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-atoi.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-atol.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-bzero.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-close.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-creat.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ctime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ctime_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ctype_.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-div.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-environ.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-envlock.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-fclose.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-fflush.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-findfp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-fputwc.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-fvwrite.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-fwalk.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-getenv_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-gettzinfo.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-gmtime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-gmtime_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-impure.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isalnum.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isalpha.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isascii.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isblank.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-iscntrl.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isdigit.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isgraph.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-islower.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isprint.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ispunct.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isspace.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-isupper.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-itoa.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-labs.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-lcltime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-lcltime_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ldiv.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-longjmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-makebuf.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memccpy.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memchr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memcmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memcpy.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memmove.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memrchr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-memset.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-mktime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-month_lengths.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-open.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-quorem.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-raise.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-rand.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-rand_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-read.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-refill.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-rshift.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-s_fpclassify.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sbrk.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sccl.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-setjmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sf_nan.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-srand.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-stdio.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcasecmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcasestr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcat.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strchr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcoll.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcpy.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strcspn.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strdup.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strdup_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strftime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strlcat.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strlcpy.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strlen.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strlwr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strncasecmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strncat.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strncmp.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strncpy.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strndup.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strndup_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strnlen.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strptime.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strrchr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strsep.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strspn.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strstr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strtok_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strtol.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strtoul.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-strupr.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sysclose.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sysopen.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-sysread.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-syssbrk.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-system.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-systimes.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-syswrite.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-time.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-timelocal.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-toascii.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tolower.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-toupper.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tzcalc_limits.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tzlock.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tzset.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tzset_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-tzvars.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-ungetc.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-utoa.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-wbuf.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-wcrtomb.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-wctomb_r.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lib_a-wsetup.*( .literal  .literal.*  .text  .text.*)
    *libc.a:lock.*( .literal  .literal.*  .text  .text.*)
    *libdriver.a:gpio.*(.literal.gpio_iomux_in .text.gpio_iomux_in)
    *libdriver.a:gpio.*(.literal.gpio_iomux_out .text.gpio_iomux_out)
    *libesp32.a:panic.*( .literal  .literal.*  .text  .text.*)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_isr_post .text.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.literal.esp_event_isr_post_to .text.esp_event_isr_post_to)
    *libesp_gdbstub.a:( .literal  .literal.*  .text  .text.*)
    *libesp_ringbuf.a:( .literal  .literal.*  .text  .text.*)
    *libespcoredump.a:core_dump_common.*( .literal  .literal.*  .text  .text.*)
    *libespcoredump.a:core_dump_flash.*( .literal  .literal.*  .text  .text.*)
    *libespcoredump.a:core_dump_port.*( .literal  .literal.*  .text  .text.*)
    *libespcoredump.a:core_dump_uart.*( .literal  .literal.*  .text  .text.*)
    *libfreertos.a:( .literal EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .literal.*  .text EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .text.*)
    *libfreertos.a:port.*(.iram1.4.literal .iram1.4 .literal.vPortCPUReleaseMutexIntsDisabledExtram .literal.vPortCPUReleaseMutexIntsDisabledInternal .literal.xPortSysTickHandler .literal.vPortYieldOtherCore .literal.vPortReleaseTaskMPUSettings .literal.xPortInIsrContext .literal.vPortAssertIfInISR .literal.vPortCPUInitializeMutex .literal.vPortCPUReleaseMutex .literal.vPortSetStackWatchpoint .literal.uxPortCompareSetExtram .literal.vPortCPUAcquireMutex .literal.vPortCPUAcquireMutexTimeout .text.vPortCPUReleaseMutexIntsDisabledExtram .text.vPortCPUReleaseMutexIntsDisabledInternal .text.vPortEndScheduler .text.xPortSysTickHandler .text.vPortYieldOtherCore .text.vPortStoreTaskMPUSettings .text.vPortReleaseTaskMPUSettings .text.xPortInIsrContext .text.vPortAssertIfInISR .text.vPortCPUInitializeMutex .text.vPortCPUReleaseMutex .text.vPortSetStackWatchpoint .text.uxPortCompareSetExtram .text.vPortCPUAcquireMutex .text.vPortCPUAcquireMutexTimeout .text.xPortGetTickRateHz)
    *libfreertos.a:queue.*( .iram1.* .literal.prvCopyDataToQueue .literal.prvCopyDataFromQueue .literal.prvNotifyQueueSetContainer .literal.xQueueGenericReset .literal.xQueueGenericCreate .literal.xQueueGetMutexHolder .literal.xQueueCreateCountingSemaphoreStatic .literal.xQueueCreateCountingSemaphore .literal.xQueueGenericSend .literal.prvInitialiseMutex$part$1 .literal.xQueueCreateMutexStatic .literal.xQueueCreateMutex .literal.xQueueGiveMutexRecursive .literal.xQueueGenericSendFromISR .literal.xQueueGiveFromISR .literal.xQueueGenericReceive .literal.xQueueTakeMutexRecursive .literal.xQueueReceiveFromISR .literal.xQueuePeekFromISR .literal.uxQueueMessagesWaiting .literal.uxQueueSpacesAvailable .literal.uxQueueMessagesWaitingFromISR .literal.vQueueDelete .literal.xQueueIsQueueEmptyFromISR .literal.xQueueIsQueueFullFromISR .literal.vQueueWaitForMessageRestricted .literal.xQueueCreateSet .literal.xQueueAddToSet .literal.xQueueRemoveFromSet .literal.xQueueSelectFromSet .literal.xQueueSelectFromSetFromISR .text.prvCopyDataToQueue .text.prvCopyDataFromQueue .text.prvNotifyQueueSetContainer .text.xQueueGenericReset .text.xQueueGenericCreate .text.xQueueGetMutexHolder .text.xQueueCreateCountingSemaphoreStatic .text.xQueueCreateCountingSemaphore .text.xQueueGenericSend .text.prvInitialiseMutex$part$1 .text.xQueueCreateMutexStatic .text.xQueueCreateMutex .text.xQueueGiveMutexRecursive .text.xQueueGenericSendFromISR .text.xQueueGiveFromISR .text.xQueueGenericReceive .text.xQueueTakeMutexRecursive .text.xQueueReceiveFromISR .text.xQueuePeekFromISR .text.uxQueueMessagesWaiting .text.uxQueueSpacesAvailable .text.uxQueueMessagesWaitingFromISR .text.vQueueDelete .text.xQueueIsQueueEmptyFromISR .text.xQueueIsQueueFullFromISR .text.vQueueWaitForMessageRestricted .text.xQueueCreateSet .text.xQueueAddToSet .text.xQueueRemoveFromSet .text.xQueueSelectFromSet .text.xQueueSelectFromSetFromISR)
    *libfreertos.a:tasks.*( .iram1.* .literal.prvInitialiseNewTask .literal.prvResetNextTaskUnblockTime .literal.prvDeleteTLS .literal.prvTaskIsTaskSuspended .literal.prvDeleteTCB .literal.prvAddCurrentTaskToDelayedList .literal.taskYIELD_OTHER_CORE .literal.vTaskEndScheduler .literal.vTaskSuspendAll .literal.xTaskGetTickCount .literal.xTaskGetTickCountFromISR .literal.uxTaskGetNumberOfTasks .literal.xTaskGetIdleTaskHandle .literal.xTaskGetIdleTaskHandleForCPU .literal.vTaskSwitchContext .literal.vTaskSetTimeOutState .literal.vTaskMissedYield .literal.vTaskAllocateMPURegions .literal.xTaskGetCurrentTaskHandle .literal.__getreent .literal.pcTaskGetTaskName .literal.pvTaskGetThreadLocalStoragePointer .literal.xTaskGetAffinity .literal.uxTaskGetStackHighWaterMark .literal.pxTaskGetStackStart .literal.xTaskGetCurrentTaskHandleForCPU .literal.xTaskGetSchedulerState .literal.vTaskEnterCritical .literal.vTaskExitCritical .literal.prvAddNewTaskToReadyList$isra$9 .literal.xTaskCreateStaticPinnedToCore .literal.xTaskCreateRestricted .literal.xTaskCreatePinnedToCore .literal.vTaskDelete .literal.vTaskDelayUntil .literal.vTaskDelay .literal.eTaskGetState .literal.uxTaskPriorityGet .literal.uxTaskPriorityGetFromISR .literal.vTaskPrioritySet .literal.vTaskSuspend .literal.vTaskResume .literal.xTaskResumeFromISR .literal.prvIdleTask .literal.xTaskIncrementTick .literal.xTaskResumeAll .literal.vTaskPlaceOnEventList .literal.vTaskPlaceOnUnorderedEventList .literal.vTaskPlaceOnEventListRestricted .literal.xTaskRemoveFromEventList .literal.xTaskRemoveFromUnorderedEventList .literal.xTaskCheckForTimeOut .literal.vTaskSetThreadLocalStoragePointerAndDelCallback .literal.vTaskSetThreadLocalStoragePointer .literal.vTaskPriorityInherit .literal.xTaskPriorityDisinherit .literal.uxTaskResetEventItemValue .literal.pvTaskIncrementMutexHeldCount .literal.ulTaskNotifyTake .literal.xTaskNotifyWait .literal.xTaskNotify .literal.xTaskNotifyFromISR .literal.vTaskNotifyGiveFromISR .literal.uxTaskGetSnapshotAll .text.prvInitialiseNewTask .text.prvResetNextTaskUnblockTime .text.prvTaskGetSnapshotsFromList .text.prvDeleteTLS .text.prvTaskIsTaskSuspended .text.prvDeleteTCB .text.prvAddCurrentTaskToDelayedList .text.taskYIELD_OTHER_CORE .text.vTaskEndScheduler .text.vTaskSuspendAll .text.xTaskGetTickCount .text.xTaskGetTickCountFromISR .text.uxTaskGetNumberOfTasks .text.xTaskGetIdleTaskHandle .text.xTaskGetIdleTaskHandleForCPU .text.vTaskSwitchContext .text.vTaskSetTimeOutState .text.vTaskMissedYield .text.vTaskAllocateMPURegions .text.xTaskGetCurrentTaskHandle .text.__getreent .text.pcTaskGetTaskName .text.pvTaskGetThreadLocalStoragePointer .text.xTaskGetAffinity .text.uxTaskGetStackHighWaterMark .text.pxTaskGetStackStart .text.xTaskGetCurrentTaskHandleForCPU .text.xTaskGetSchedulerState .text.vTaskEnterCritical .text.vTaskExitCritical .text.prvAddNewTaskToReadyList$isra$9 .text.xTaskCreateStaticPinnedToCore .text.xTaskCreateRestricted .text.xTaskCreatePinnedToCore .text.vTaskDelete .text.vTaskDelayUntil .text.vTaskDelay .text.eTaskGetState .text.uxTaskPriorityGet .text.uxTaskPriorityGetFromISR .text.vTaskPrioritySet .text.vTaskSuspend .text.vTaskResume .text.xTaskResumeFromISR .text.prvIdleTask .text.xTaskIncrementTick .text.xTaskResumeAll .text.vTaskPlaceOnEventList .text.vTaskPlaceOnUnorderedEventList .text.vTaskPlaceOnEventListRestricted .text.xTaskRemoveFromEventList .text.xTaskRemoveFromUnorderedEventList .text.xTaskCheckForTimeOut .text.vTaskSetThreadLocalStoragePointerAndDelCallback .text.vTaskSetThreadLocalStoragePointer .text.vTaskPriorityInherit .text.xTaskPriorityDisinherit .text.uxTaskResetEventItemValue .text.pvTaskIncrementMutexHeldCount .text.ulTaskNotifyTake .text.xTaskNotifyWait .text.xTaskNotify .text.xTaskNotifyFromISR .text.vTaskNotifyGiveFromISR .text.uxTaskGetSnapshotAll)
    *libfreertos.a:port.*(.iram1.pxPortInitialiseStack)
    *libfreertos.a:port.*(.iram1.xPortStartScheduler)
    *libfreertos.a:queue.*(.iram1.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.iram1.vTaskStartScheduler)
    *libgcc.a:_divsf3.*( .literal  .literal.*  .text  .text.*)
    *libgcc.a:lib2funcs.*( .literal  .literal.*  .text  .text.*)
    *libgcov.a:( .literal  .literal.*  .text  .text.*)
    *libhal.a:( .literal  .literal.*  .text  .text.*)
    *libheap.a:multi_heap.*( .literal  .literal.*  .text  .text.*)
    *libnewlib.a:heap.*( .literal  .literal.*  .text  .text.*)
    *librtc.a:( .literal  .literal.*  .text  .text.*)
    *libsoc.a:cpu_util.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:lldesc.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_clk.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_clk_init.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_init.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_pm.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_sleep.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_time.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_wdt.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_flash_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_slave_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:memspi_host_driver.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_gd.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_generic.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_issi.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_rom_patch.*( .literal  .literal.*  .text  .text.*)
    *libxtensa.a:eri.*( .literal  .literal.*  .text  .text.*)

    _iram_text_end = ABSOLUTE(.);
    _iram_end = ABSOLUTE(.);
  } > iram0_0_seg

  ASSERT(((_iram_text_end - ORIGIN(iram0_0_seg)) <= LENGTH(iram0_0_seg)),
          "IRAM0 segment data does not fit.")

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    _bt_data_start = ABSOLUTE(.);
    *libbt.a:(.data .data.*)
    . = ALIGN (4);
    _bt_data_end = ABSOLUTE(.);
    _btdm_data_start = ABSOLUTE(.);
    *libbtdm_app.a:(.data .data.*)
    . = ALIGN (4);
    _btdm_data_end = ABSOLUTE(.);
    _nimble_data_start = ABSOLUTE(.);
    *libnimble.a:(.data .data.*)
    . = ALIGN (4);
    _nimble_data_end = ABSOLUTE(.);
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)

    *( .data EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .data.*  .dram1 EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .dram1.*)
    *libapp_trace.a:app_trace.*( .rodata  .rodata.*)
    *libapp_trace.a:app_trace_util.*( .rodata  .rodata.*)
    *libc.a:creat.*( .rodata  .rodata.*)
    *libc.a:isatty.*( .rodata  .rodata.*)
    *libc.a:lib_a-abs.*( .rodata  .rodata.*)
    *libc.a:lib_a-asctime.*( .rodata  .rodata.*)
    *libc.a:lib_a-asctime_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-atoi.*( .rodata  .rodata.*)
    *libc.a:lib_a-atol.*( .rodata  .rodata.*)
    *libc.a:lib_a-bzero.*( .rodata  .rodata.*)
    *libc.a:lib_a-close.*( .rodata  .rodata.*)
    *libc.a:lib_a-creat.*( .rodata  .rodata.*)
    *libc.a:lib_a-ctime.*( .rodata  .rodata.*)
    *libc.a:lib_a-ctime_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-ctype_.*( .rodata  .rodata.*)
    *libc.a:lib_a-div.*( .rodata  .rodata.*)
    *libc.a:lib_a-environ.*( .rodata  .rodata.*)
    *libc.a:lib_a-envlock.*( .rodata  .rodata.*)
    *libc.a:lib_a-fclose.*( .rodata  .rodata.*)
    *libc.a:lib_a-fflush.*( .rodata  .rodata.*)
    *libc.a:lib_a-findfp.*( .rodata  .rodata.*)
    *libc.a:lib_a-fputwc.*( .rodata  .rodata.*)
    *libc.a:lib_a-fvwrite.*( .rodata  .rodata.*)
    *libc.a:lib_a-fwalk.*( .rodata  .rodata.*)
    *libc.a:lib_a-getenv_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-gettzinfo.*( .rodata  .rodata.*)
    *libc.a:lib_a-gmtime.*( .rodata  .rodata.*)
    *libc.a:lib_a-gmtime_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-impure.*( .rodata  .rodata.*)
    *libc.a:lib_a-isalnum.*( .rodata  .rodata.*)
    *libc.a:lib_a-isalpha.*( .rodata  .rodata.*)
    *libc.a:lib_a-isascii.*( .rodata  .rodata.*)
    *libc.a:lib_a-isblank.*( .rodata  .rodata.*)
    *libc.a:lib_a-iscntrl.*( .rodata  .rodata.*)
    *libc.a:lib_a-isdigit.*( .rodata  .rodata.*)
    *libc.a:lib_a-isgraph.*( .rodata  .rodata.*)
    *libc.a:lib_a-islower.*( .rodata  .rodata.*)
    *libc.a:lib_a-isprint.*( .rodata  .rodata.*)
    *libc.a:lib_a-ispunct.*( .rodata  .rodata.*)
    *libc.a:lib_a-isspace.*( .rodata  .rodata.*)
    *libc.a:lib_a-isupper.*( .rodata  .rodata.*)
    *libc.a:lib_a-itoa.*( .rodata  .rodata.*)
    *libc.a:lib_a-labs.*( .rodata  .rodata.*)
    *libc.a:lib_a-lcltime.*( .rodata  .rodata.*)
    *libc.a:lib_a-lcltime_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-ldiv.*( .rodata  .rodata.*)
    *libc.a:lib_a-longjmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-makebuf.*( .rodata  .rodata.*)
    *libc.a:lib_a-memccpy.*( .rodata  .rodata.*)
    *libc.a:lib_a-memchr.*( .rodata  .rodata.*)
    *libc.a:lib_a-memcmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-memcpy.*( .rodata  .rodata.*)
    *libc.a:lib_a-memmove.*( .rodata  .rodata.*)
    *libc.a:lib_a-memrchr.*( .rodata  .rodata.*)
    *libc.a:lib_a-memset.*( .rodata  .rodata.*)
    *libc.a:lib_a-mktime.*( .rodata  .rodata.*)
    *libc.a:lib_a-month_lengths.*( .rodata  .rodata.*)
    *libc.a:lib_a-open.*( .rodata  .rodata.*)
    *libc.a:lib_a-quorem.*( .rodata  .rodata.*)
    *libc.a:lib_a-raise.*( .rodata  .rodata.*)
    *libc.a:lib_a-rand.*( .rodata  .rodata.*)
    *libc.a:lib_a-rand_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-read.*( .rodata  .rodata.*)
    *libc.a:lib_a-refill.*( .rodata  .rodata.*)
    *libc.a:lib_a-rshift.*( .rodata  .rodata.*)
    *libc.a:lib_a-s_fpclassify.*( .rodata  .rodata.*)
    *libc.a:lib_a-sbrk.*( .rodata  .rodata.*)
    *libc.a:lib_a-sccl.*( .rodata  .rodata.*)
    *libc.a:lib_a-setjmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-sf_nan.*( .rodata  .rodata.*)
    *libc.a:lib_a-srand.*( .rodata  .rodata.*)
    *libc.a:lib_a-stdio.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcasecmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcasestr.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcat.*( .rodata  .rodata.*)
    *libc.a:lib_a-strchr.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcoll.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcpy.*( .rodata  .rodata.*)
    *libc.a:lib_a-strcspn.*( .rodata  .rodata.*)
    *libc.a:lib_a-strdup.*( .rodata  .rodata.*)
    *libc.a:lib_a-strdup_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-strftime.*( .rodata  .rodata.*)
    *libc.a:lib_a-strlcat.*( .rodata  .rodata.*)
    *libc.a:lib_a-strlcpy.*( .rodata  .rodata.*)
    *libc.a:lib_a-strlen.*( .rodata  .rodata.*)
    *libc.a:lib_a-strlwr.*( .rodata  .rodata.*)
    *libc.a:lib_a-strncasecmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-strncat.*( .rodata  .rodata.*)
    *libc.a:lib_a-strncmp.*( .rodata  .rodata.*)
    *libc.a:lib_a-strncpy.*( .rodata  .rodata.*)
    *libc.a:lib_a-strndup.*( .rodata  .rodata.*)
    *libc.a:lib_a-strndup_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-strnlen.*( .rodata  .rodata.*)
    *libc.a:lib_a-strptime.*( .rodata  .rodata.*)
    *libc.a:lib_a-strrchr.*( .rodata  .rodata.*)
    *libc.a:lib_a-strsep.*( .rodata  .rodata.*)
    *libc.a:lib_a-strspn.*( .rodata  .rodata.*)
    *libc.a:lib_a-strstr.*( .rodata  .rodata.*)
    *libc.a:lib_a-strtok_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-strtol.*( .rodata  .rodata.*)
    *libc.a:lib_a-strtoul.*( .rodata  .rodata.*)
    *libc.a:lib_a-strupr.*( .rodata  .rodata.*)
    *libc.a:lib_a-sysclose.*( .rodata  .rodata.*)
    *libc.a:lib_a-sysopen.*( .rodata  .rodata.*)
    *libc.a:lib_a-sysread.*( .rodata  .rodata.*)
    *libc.a:lib_a-syssbrk.*( .rodata  .rodata.*)
    *libc.a:lib_a-system.*( .rodata  .rodata.*)
    *libc.a:lib_a-systimes.*( .rodata  .rodata.*)
    *libc.a:lib_a-syswrite.*( .rodata  .rodata.*)
    *libc.a:lib_a-time.*( .rodata  .rodata.*)
    *libc.a:lib_a-timelocal.*( .rodata  .rodata.*)
    *libc.a:lib_a-toascii.*( .rodata  .rodata.*)
    *libc.a:lib_a-tolower.*( .rodata  .rodata.*)
    *libc.a:lib_a-toupper.*( .rodata  .rodata.*)
    *libc.a:lib_a-tzcalc_limits.*( .rodata  .rodata.*)
    *libc.a:lib_a-tzlock.*( .rodata  .rodata.*)
    *libc.a:lib_a-tzset.*( .rodata  .rodata.*)
    *libc.a:lib_a-tzset_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-tzvars.*( .rodata  .rodata.*)
    *libc.a:lib_a-ungetc.*( .rodata  .rodata.*)
    *libc.a:lib_a-utoa.*( .rodata  .rodata.*)
    *libc.a:lib_a-wbuf.*( .rodata  .rodata.*)
    *libc.a:lib_a-wcrtomb.*( .rodata  .rodata.*)
    *libc.a:lib_a-wctomb_r.*( .rodata  .rodata.*)
    *libc.a:lib_a-wsetup.*( .rodata  .rodata.*)
    *libc.a:lock.*( .rodata  .rodata.*)
    *libdriver.a:gpio.*(.rodata.gpio_iomux_in)
    *libdriver.a:gpio.*(.rodata.gpio_iomux_out)
    *libesp32.a:panic.*( .rodata  .rodata.*)
    *libesp_event.a:default_event_loop.*(.rodata.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.rodata.esp_event_isr_post_to)
    *libesp_gdbstub.a:( .rodata  .rodata.*)
    *libfreertos.a:port.*(.data.extram_mux  .dram1.*)
    *libfreertos.a:queue.*( .data.*  .dram1.*)
    *libfreertos.a:tasks.*(.data.xTaskQueueMutex .data.xNextTaskUnblockTime  .dram1.*)
    *libfreertos.a:port.*(.data.pxPortInitialiseStack .dram1.pxPortInitialiseStack)
    *libfreertos.a:port.*(.data.xPortStartScheduler .dram1.xPortStartScheduler)
    *libfreertos.a:queue.*(.data.xQueueGenericCreateStatic .dram1.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.data.vTaskStartScheduler .dram1.vTaskStartScheduler)
    *libgcc.a:_divsf3.*( .rodata  .rodata.*)
    *libgcov.a:( .rodata  .rodata.*)
    *libheap.a:multi_heap.*( .rodata  .rodata.*)
    *libnewlib.a:heap.*( .rodata  .rodata.*)
    *libphy.a:( .rodata  .rodata.*)
    *libsoc.a:rtc_clk.*( .rodata  .rodata.*)
    *libsoc.a:spi_flash_hal_iram.*( .rodata  .rodata.*)
    *libspi_flash.a:memspi_host_driver.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_gd.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_generic.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_issi.*( .rodata  .rodata.*)

    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > dram0_0_seg

  /*This section holds data that should not be initialized at power up.
    The section located in Internal SRAM memory region. The macro _NOINIT
    can be used as attribute to place data into this section.
    See the esp_attr.h file for more information.
  */
  .noinit (NOLOAD):
  {
    . = ALIGN(4);
    _noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _noinit_end = ABSOLUTE(.);
  } > dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.ext_ram.bss*)
    _bt_bss_start = ABSOLUTE(.);
    *libbt.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _bt_bss_end = ABSOLUTE(.);
    _btdm_bss_start = ABSOLUTE(.);
    *libbtdm_app.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _btdm_bss_end = ABSOLUTE(.);
    _nimble_bss_start = ABSOLUTE(.);
    *libnimble.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _nimble_bss_end = ABSOLUTE(.);

    *( .bss EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .bss.*  COMMON)
    *libfreertos.a:port.*(.bss.port_interruptNesting .bss.port_xSchedulerRunning)
    *libfreertos.a:queue.*( .bss.*)
    *libfreertos.a:tasks.*(.bss.xSwitchingContext .bss.uxSchedulerSuspended .bss.uxTaskNumber .bss.xNumOfOverflows .bss.xYieldPending .bss.uxPendedTicks .bss.xSchedulerRunning .bss.uxTopReadyPriority .bss.xTickCount .bss.uxCurrentNumberOfTasks .bss.xIdleTaskHandle .bss.xSuspendedTaskList .bss.uxTasksDeleted .bss.xTasksWaitingTermination .bss.xPendingReadyList .bss.pxOverflowDelayedTaskList .bss.pxDelayedTaskList .bss.xDelayedTaskList2 .bss.xDelayedTaskList1 .bss.pxReadyTasksLists .bss.pxCurrentTCB)
    *libfreertos.a:port.*(.bss.pxPortInitialiseStack)
    *libfreertos.a:port.*(.bss.xPortStartScheduler)
    *libfreertos.a:queue.*(.bss.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.bss.vTaskStartScheduler)

    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.share.mem)
    *(.gnu.linkonce.b.*)

    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } > dram0_0_seg

  ASSERT(((_bss_end - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")

  .flash.rodata :
  {
    _rodata_start = ABSOLUTE(.);

    *(.rodata_desc .rodata_desc.*)               /* Should be the first.  App version info.        DO NOT PUT ANYTHING BEFORE IT! */
    *(.rodata_custom_desc .rodata_custom_desc.*) /* Should be the second. Custom app version info. DO NOT PUT ANYTHING BEFORE IT! */

    *(EXCLUDE_FILE(*libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_gd.* *libphy.a *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcov.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .rodata EXCLUDE_FILE(*libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_chip_gd.* *libdriver.a:gpio.* *libphy.a *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcov.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.* *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .rodata.*)
    *libdriver.a:gpio.*(.rodata.gpio_output_disable.str1.1 .rodata.gpio_output_enable.str1.1 .rodata.gpio_pullup_en.str1.1 .rodata.gpio_pullup_dis.str1.1 .rodata.gpio_set_intr_type.str1.1 .rodata.gpio_set_pull_mode.str1.1 .rodata.gpio_set_direction.str1.1 .rodata.gpio_config.str1.1 .rodata.gpio_reset_pin.str1.1 .rodata.gpio_isr_handler_add.str1.1 .rodata.gpio_isr_register.str1.1 .rodata.gpio_install_isr_service.str1.1 .rodata.gpio_wakeup_enable.str1.1 .rodata.gpio_set_drive_capability.str1.1 .rodata.gpio_get_drive_capability.str1.1 .rodata.gpio_hold_en.str1.1 .rodata.gpio_iomux_in.str1.1 .rodata.gpio_iomux_out.str1.1 .rodata.__func__$5885 .rodata.__func__$5876 .rodata.__FUNCTION__$5852 .rodata.__FUNCTION__$5843 .rodata.__func__$5836 .rodata.__FUNCTION__$5835 .rodata.__func__$5827 .rodata.__FUNCTION__$5826 .rodata.__FUNCTION__$5820 .rodata.__FUNCTION__$5815 .rodata.__FUNCTION__$5808 .rodata.__FUNCTION__$5793 .rodata.__FUNCTION__$5789 .rodata.__FUNCTION__$5785 .rodata.__func__$5764 .rodata.__func__$5749 .rodata.__func__$5699 .rodata.__FUNCTION__$5698 .rodata.__FUNCTION__$5704 .rodata.__func__$5731 .rodata.__FUNCTION__$5729 .rodata.__FUNCTION__$5717 .rodata.__FUNCTION__$5709 .rodata.__FUNCTION__$5694 .rodata.__FUNCTION__$5690 .rodata.__FUNCTION__$5679 .rodata.__func__$5673 .rodata.__FUNCTION__$5672 .rodata.__func__$5667 .rodata.__FUNCTION__$5666 .rodata.__func__$5661 .rodata.__FUNCTION__$5660 .rodata.__func__$5655 .rodata.__FUNCTION__$5654 .rodata.GPIO_HOLD_MASK)
    *libesp_event.a:default_event_loop.*(.rodata.esp_event_loop_create_default.str1.1 .rodata.esp_event_send_to_default_loop.str1.1 .rodata.esp_event_send_to_default_loop)
    *libesp_event.a:esp_event.*(.rodata.handler_instances_add$isra$2.str1.1 .rodata.base_node_add_handler.str1.1 .rodata.loop_node_add_handler.str1.1 .rodata.esp_event_loop_create.str1.1 .rodata.esp_event_loop_run.str1.1 .rodata.esp_event_loop_run_task.str1.1 .rodata.esp_event_handler_register_with.str1.1 .rodata.esp_event_handler_unregister_with.str1.1 .rodata.__func__$7122 .rodata.__func__$7109 .rodata.__func__$7089 .rodata.__func__$7073 .rodata.__func__$7050 .rodata.__func__$7009)
    *libfreertos.a:port.*(.rodata.vPortCPUReleaseMutexIntsDisabledExtram.str1.1 .rodata.vPortAssertIfInISR.str1.1 .rodata.uxPortCompareSetExtram.str1.1 .rodata.__func__$4938 .rodata.__func__$4956 .rodata.__func__$4933 .rodata.__func__$4951 .rodata.__FUNCTION__$4849)
    *libfreertos.a:queue.*(.rodata.prvNotifyQueueSetContainer.str1.1 .rodata.__FUNCTION__$5005 .rodata.__FUNCTION__$4995 .rodata.__FUNCTION__$4975 .rodata.__FUNCTION__$4970 .rodata.__FUNCTION__$4964 .rodata.__FUNCTION__$4958 .rodata.__FUNCTION__$4952 .rodata.__FUNCTION__$4943 .rodata.__FUNCTION__$4933 .rodata.__FUNCTION__$4922 .rodata.__FUNCTION__$4914 .rodata.__FUNCTION__$5041 .rodata.__FUNCTION__$4903 .rodata.__FUNCTION__$4892 .rodata.__FUNCTION__$4886 .rodata.__FUNCTION__$4879 .rodata.__FUNCTION__$4872 .rodata.__FUNCTION__$4838 .rodata.__FUNCTION__$4828 .rodata.__FUNCTION__$4819)
    *libfreertos.a:tasks.*(.rodata.prvDeleteTLS.str1.1 .rodata.vTaskAllocateMPURegions.str1.1 .rodata.vTaskEnterCritical.str1.1 .rodata.vTaskExitCritical.str1.1 .rodata.vTaskStartScheduler.str1.1 .rodata.__FUNCTION__$5518 .rodata.__FUNCTION__$5505 .rodata.__FUNCTION__$5489 .rodata.__func__$5421 .rodata.__func__$5439 .rodata.__func__$5416 .rodata.__func__$5434 .rodata.__FUNCTION__$5403 .rodata.__FUNCTION__$5313 .rodata.__FUNCTION__$5278 .rodata.__FUNCTION__$5272 .rodata.__FUNCTION__$5268 .rodata.__FUNCTION__$5258 .rodata.__FUNCTION__$5249 .rodata.__FUNCTION__$5243 .rodata.__FUNCTION__$5236 .rodata.ucExpectedStackBytes$5208 .rodata.__FUNCTION__$5200 .rodata.__FUNCTION__$5191 .rodata.__FUNCTION__$5186 .rodata.__FUNCTION__$5182 .rodata.__FUNCTION__$5162 .rodata.__FUNCTION__$5144 .rodata.__FUNCTION__$5132 .rodata.__FUNCTION__$5121 .rodata.__FUNCTION__$5126 .rodata.__FUNCTION__$5115 .rodata.__FUNCTION__$5109 .rodata.__FUNCTION__$5090 .rodata.__FUNCTION__$5081 .rodata.__FUNCTION__$5074 .rodata.__FUNCTION__$5367 .rodata.__FUNCTION__$5371 .rodata.__FUNCTION__$5066 .rodata.__FUNCTION__$5017 .rodata.__FUNCTION__$5059 .rodata.__FUNCTION__$5010)
    *libfreertos.a:port.*(.rodata.pxPortInitialiseStack)
    *libfreertos.a:port.*(.rodata.xPortStartScheduler)
    *libfreertos.a:queue.*(.rodata.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.rodata.vTaskStartScheduler)

    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /*  C++ constructor and destructor tables

        Make a point of not including anything from crtbegin.o or crtend.o, as IDF doesn't use toolchain crt
      */
    __init_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .ctors SORT(.ctors.*)))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    /* Addresses of memory regions reserved via
       SOC_RESERVE_MEMORY_REGION() */
    soc_reserved_memory_region_start = ABSOLUTE(.);
    KEEP (*(.reserved_memory_address))
    soc_reserved_memory_region_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >drom0_0_seg

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libesp_ringbuf.a *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libspi_flash.a:spi_flash_chip_gd.* *librtc.a *libespcoredump.a:core_dump_port.* *libespcoredump.a:core_dump_common.* *libespcoredump.a:core_dump_flash.* *libespcoredump.a:core_dump_uart.* *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libgcov.a *libxtensa.a:eri.* *libhal.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:lldesc.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:rtc_time.* *libsoc.a:rtc_sleep.* *libsoc.a:rtc_init.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libfreertos.a *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .literal EXCLUDE_FILE(*libesp_ringbuf.a *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libspi_flash.a:spi_flash_chip_gd.* *libdriver.a:gpio.* *librtc.a *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libespcoredump.a:core_dump_port.* *libespcoredump.a:core_dump_common.* *libespcoredump.a:core_dump_flash.* *libespcoredump.a:core_dump_uart.* *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libgcov.a *libxtensa.a:eri.* *libhal.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:lldesc.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:rtc_time.* *libsoc.a:rtc_sleep.* *libsoc.a:rtc_init.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.* *libfreertos.a *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .literal.*  .phyiram EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .phyiram.* EXCLUDE_FILE(*libesp_ringbuf.a *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libspi_flash.a:spi_flash_chip_gd.* *librtc.a *libespcoredump.a:core_dump_port.* *libespcoredump.a:core_dump_common.* *libespcoredump.a:core_dump_flash.* *libespcoredump.a:core_dump_uart.* *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libgcov.a *libxtensa.a:eri.* *libhal.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:lldesc.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:rtc_time.* *libsoc.a:rtc_sleep.* *libsoc.a:rtc_init.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libfreertos.a *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .text EXCLUDE_FILE(*libesp_ringbuf.a *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libspi_flash.a:spi_flash_chip_gd.* *libdriver.a:gpio.* *librtc.a *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libespcoredump.a:core_dump_port.* *libespcoredump.a:core_dump_common.* *libespcoredump.a:core_dump_flash.* *libespcoredump.a:core_dump_uart.* *libesp32.a:panic.* *libgcc.a:_divsf3.* *libgcc.a:lib2funcs.* *libgcov.a *libxtensa.a:eri.* *libhal.a *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:lldesc.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:rtc_time.* *libsoc.a:rtc_sleep.* *libsoc.a:rtc_init.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_clk.* *libheap.a:multi_heap.* *libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.* *libfreertos.a *libc.a:lib_a-fclose.* *libc.a:lib_a-iscntrl.* *libc.a:lib_a-strspn.* *libc.a:lib_a-strcoll.* *libc.a:lib_a-isspace.* *libc.a:lib_a-strrchr.* *libc.a:lib_a-getenv_r.* *libc.a:lib_a-stdio.* *libc.a:lib_a-ldiv.* *libc.a:lib_a-isalpha.* *libc.a:lib_a-strncmp.* *libc.a:lib_a-lcltime.* *libc.a:lib_a-bzero.* *libc.a:lib_a-strptime.* *libc.a:lib_a-strcasestr.* *libc.a:lib_a-toascii.* *libc.a:lib_a-tolower.* *libc.a:lib_a-labs.* *libc.a:lib_a-strftime.* *libc.a:lib_a-isupper.* *libc.a:lib_a-asctime.* *libc.a:lib_a-div.* *libc.a:lib_a-longjmp.* *libc.a:lib_a-makebuf.* *libc.a:lib_a-fwalk.* *libc.a:lib_a-strdup_r.* *libc.a:lib_a-environ.* *libc.a:lib_a-fputwc.* *libc.a:lib_a-refill.* *libc.a:lib_a-strstr.* *libc.a:lib_a-strcat.* *libc.a:lib_a-srand.* *libc.a:lib_a-timelocal.* *libc.a:lib_a-strtoul.* *libc.a:lib_a-atol.* *libc.a:lib_a-setjmp.* *libc.a:lib_a-raise.* *libc.a:lib_a-wsetup.* *libc.a:lib_a-wbuf.* *libc.a:lib_a-s_fpclassify.* *libc.a:lib_a-systimes.* *libc.a:creat.* *libc.a:lib_a-ispunct.* *libc.a:lib_a-ctype_.* *libc.a:lib_a-strncasecmp.* *libc.a:lib_a-memccpy.* *libc.a:lib_a-strcspn.* *libc.a:lib_a-close.* *libc.a:lib_a-mktime.* *libc.a:lock.* *libc.a:lib_a-memmove.* *libc.a:lib_a-strnlen.* *libc.a:lib_a-gmtime.* *libc.a:lib_a-memset.* *libc.a:lib_a-sysopen.* *libc.a:lib_a-creat.* *libc.a:lib_a-time.* *libc.a:lib_a-utoa.* *libc.a:lib_a-strsep.* *libc.a:lib_a-tzvars.* *libc.a:lib_a-tzlock.* *libc.a:lib_a-strtol.* *libc.a:lib_a-tzset_r.* *libc.a:lib_a-memcmp.* *libc.a:lib_a-isalnum.* *libc.a:lib_a-gettzinfo.* *libc.a:lib_a-isblank.* *libc.a:lib_a-strndup.* *libc.a:lib_a-sf_nan.* *libc.a:lib_a-strlen.* *libc.a:lib_a-atoi.* *libc.a:lib_a-rand.* *libc.a:isatty.* *libc.a:lib_a-strupr.* *libc.a:lib_a-strchr.* *libc.a:lib_a-read.* *libc.a:lib_a-strcasecmp.* *libc.a:lib_a-strcmp.* *libc.a:lib_a-envlock.* *libc.a:lib_a-findfp.* *libc.a:lib_a-strtok_r.* *libc.a:lib_a-syssbrk.* *libc.a:lib_a-isascii.* *libc.a:lib_a-syswrite.* *libc.a:lib_a-memcpy.* *libc.a:lib_a-strncpy.* *libc.a:lib_a-strndup_r.* *libc.a:lib_a-isprint.* *libc.a:lib_a-strlwr.* *libc.a:lib_a-strdup.* *libc.a:lib_a-memchr.* *libc.a:lib_a-system.* *libc.a:lib_a-sbrk.* *libc.a:lib_a-rshift.* *libc.a:lib_a-memrchr.* *libc.a:lib_a-tzset.* *libc.a:lib_a-strncat.* *libc.a:lib_a-ctime_r.* *libc.a:lib_a-wcrtomb.* *libc.a:lib_a-quorem.* *libc.a:lib_a-sccl.* *libc.a:lib_a-isdigit.* *libc.a:lib_a-sysread.* *libc.a:lib_a-asctime_r.* *libc.a:lib_a-strlcat.* *libc.a:lib_a-strcpy.* *libc.a:lib_a-wctomb_r.* *libc.a:lib_a-toupper.* *libc.a:lib_a-ctime.* *libc.a:lib_a-islower.* *libc.a:lib_a-impure.* *libc.a:lib_a-fflush.* *libc.a:lib_a-fvwrite.* *libc.a:lib_a-isgraph.* *libc.a:lib_a-strlcpy.* *libc.a:lib_a-ungetc.* *libc.a:lib_a-tzcalc_limits.* *libc.a:lib_a-rand_r.* *libc.a:lib_a-month_lengths.* *libc.a:lib_a-gmtime_r.* *libc.a:lib_a-lcltime_r.* *libc.a:lib_a-itoa.* *libc.a:lib_a-open.* *libc.a:lib_a-sysclose.* *libc.a:lib_a-abs.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.* *libesp_gdbstub.a) .text.*  .wifi0iram EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .wifi0iram.*  .wifirxiram EXCLUDE_FILE(*libfreertos.a:tasks.* *libfreertos.a:port.* *libfreertos.a:queue.*) .wifirxiram.*)
    *libdriver.a:gpio.*(.literal.rtc_gpio_is_valid_gpio .literal.gpio_isr_register_on_core_static .literal.gpio_output_disable .literal.gpio_output_enable .literal.gpio_intr_enable_on_core .literal.gpio_pullup_en .literal.gpio_pullup_dis .literal.gpio_pulldown_en .literal.gpio_pulldown_dis .literal.gpio_set_intr_type .literal.gpio_intr_enable .literal.gpio_intr_disable .literal.gpio_set_level .literal.gpio_get_level .literal.gpio_set_pull_mode .literal.gpio_set_direction .literal.gpio_config .literal.gpio_reset_pin .literal.gpio_isr_handler_add .literal.gpio_isr_handler_remove .literal.gpio_uninstall_isr_service .literal.gpio_isr_register .literal.gpio_install_isr_service .literal.gpio_wakeup_enable .literal.gpio_wakeup_disable .literal.gpio_set_drive_capability .literal.gpio_get_drive_capability .literal.gpio_hold_en .literal.gpio_hold_dis .literal.gpio_deep_sleep_hold_en .literal.gpio_deep_sleep_hold_dis .text.rtc_gpio_is_valid_gpio .text.gpio_isr_register_on_core_static .text.gpio_output_disable .text.gpio_output_enable .text.gpio_intr_enable_on_core .text.gpio_pullup_en .text.gpio_pullup_dis .text.gpio_pulldown_en .text.gpio_pulldown_dis .text.gpio_set_intr_type .text.gpio_intr_enable .text.gpio_intr_disable .text.gpio_set_level .text.gpio_get_level .text.gpio_set_pull_mode .text.gpio_set_direction .text.gpio_config .text.gpio_reset_pin .text.gpio_isr_handler_add .text.gpio_isr_handler_remove .text.gpio_uninstall_isr_service .text.gpio_isr_register .text.gpio_install_isr_service .text.gpio_wakeup_enable .text.gpio_wakeup_disable .text.gpio_set_drive_capability .text.gpio_get_drive_capability .text.gpio_hold_en .text.gpio_hold_dis .text.gpio_deep_sleep_hold_en .text.gpio_deep_sleep_hold_dis)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_handler_register .literal.esp_event_handler_unregister .literal.esp_event_post .literal.esp_event_loop_create_default .literal.esp_event_loop_delete_default .literal.esp_event_send_to_default_loop .text.esp_event_handler_register .text.esp_event_handler_unregister .text.esp_event_post .text.esp_event_loop_create_default .text.esp_event_loop_delete_default .text.esp_event_send_to_default_loop)
    *libesp_event.a:esp_event.*(.literal.handler_instances_remove_all$isra$1 .literal.handler_instances_add$isra$2 .literal.base_node_add_handler .literal.loop_node_add_handler .literal.handler_instances_remove$isra$3 .literal.esp_event_loop_create .literal.esp_event_loop_run .literal.esp_event_loop_run_task .literal.esp_event_loop_delete .literal.esp_event_handler_register_with .literal.esp_event_handler_unregister_with .literal.esp_event_post_to .text.handler_instances_remove_all$isra$1 .text.handler_instances_add$isra$2 .text.base_node_add_handler .text.loop_node_add_handler .text.handler_instances_remove$isra$3 .text.esp_event_loop_create .text.esp_event_loop_run .text.esp_event_loop_run_task .text.esp_event_loop_delete .text.esp_event_handler_register_with .text.esp_event_handler_unregister_with .text.esp_event_post_to .text.esp_event_dump)
    *libfreertos.a:port.*( .phyiram.*  .wifi0iram.*  .wifirxiram.*)
    *libfreertos.a:queue.*( .phyiram.*  .wifi0iram.*  .wifirxiram.*)
    *libfreertos.a:tasks.*( .phyiram.*  .wifi0iram.*  .wifirxiram.*)
    *libfreertos.a:port.*(.literal.pxPortInitialiseStack .phyiram.pxPortInitialiseStack .text.pxPortInitialiseStack .wifi0iram.pxPortInitialiseStack .wifirxiram.pxPortInitialiseStack)
    *libfreertos.a:port.*(.literal.xPortStartScheduler .phyiram.xPortStartScheduler .text.xPortStartScheduler .wifi0iram.xPortStartScheduler .wifirxiram.xPortStartScheduler)
    *libfreertos.a:queue.*(.literal.xQueueGenericCreateStatic .phyiram.xQueueGenericCreateStatic .text.xQueueGenericCreateStatic .wifi0iram.xQueueGenericCreateStatic .wifirxiram.xQueueGenericCreateStatic)
    *libfreertos.a:tasks.*(.literal.vTaskStartScheduler .phyiram.vTaskStartScheduler .text.vTaskStartScheduler .wifi0iram.vTaskStartScheduler .wifirxiram.vTaskStartScheduler)

    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    _text_end = ABSOLUTE(.);
    _etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);
  } >iram0_2_seg
}
