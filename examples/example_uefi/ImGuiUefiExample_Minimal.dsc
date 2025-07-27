## @file
#  Minimal Platform Description file for Dear ImGui UEFI Example
#  This version is compatible with older EDK2 releases
#
##

[Defines]
  PLATFORM_NAME                  = ImGuiUefiExample
  PLATFORM_GUID                  = 87654321-4321-4321-4321-210987654321
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION               = 0x00010005
  OUTPUT_DIRECTORY                = Build/ImGuiUefiExample
  SUPPORTED_ARCHITECTURES         = IA32|X64|EBC|ARM|AARCH64
  BUILD_TARGETS                   = DEBUG|RELEASE
  SKUID_IDENTIFIER                = DEFAULT

[LibraryClasses]
  # Entry point
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  
  # Basic libraries
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  
  # UEFI & PI
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  
  # Debug (use SerialPort for older EDK2)
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  
  # Performance and Timer
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  
  # PCD
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  
  # IO Libraries
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  
  # Stack and Register Filter libraries (custom implementations)
  StackCheckLib|examples/example_uefi/StackCheckLib.inf
  RegisterFilterLib|examples/example_uefi/RegisterFilterLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  # Additional libraries for UEFI applications
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf

[Components]
  examples/example_uefi/ImGuiUefiExample.inf

[BuildOptions]
  # Global build options
  *_*_*_CC_FLAGS = -DDISABLE_NEW_DEPRECATED_INTERFACES