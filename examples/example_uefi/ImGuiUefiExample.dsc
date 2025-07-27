## @file
#  Platform Description file for Dear ImGui UEFI Example
#
##

[Defines]
  PLATFORM_NAME                  = ImGuiUefiExample
  PLATFORM_GUID                  = 87654321-4321-4321-4321-210987654321
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION               = 0x00010005
  OUTPUT_DIRECTORY                = Build/ImGuiUefiExample
  SUPPORTED_ARCHITECTURES         = IA32|IPF|X64|EBC|ARM|AARCH64
  BUILD_TARGETS                   = DEBUG|RELEASE
  SKUID_IDENTIFIER                = DEFAULT

[LibraryClasses]
  # Entry point
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  
  # Basic
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  
  # UEFI & PI
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  
  # Debug
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  
  # Performance and Timer
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  
  # PCD
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  
  # Additional libraries that might be required
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  
  # Stack and Register Filter libraries (custom implementations)
  StackCheckLib|examples/example_uefi/StackCheckLib.inf
  RegisterFilterLib|examples/example_uefi/RegisterFilterLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  # Additional libraries for UEFI applications
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

[Packages]
  MdePkg/MdePkg.dec
  MdeModulePkg/MdeModulePkg.dec
  ImGuiPkg/ImGuiPkg.dec

[Components]
  examples/example_uefi/ImGuiUefiExample.inf

[BuildOptions]
  # Global build options
  *_*_*_CC_FLAGS = -DDISABLE_NEW_DEPRECATED_INTERFACES