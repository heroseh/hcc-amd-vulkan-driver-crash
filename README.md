## System Info

Please find my system information in the **vulkaninfo.txt** file.

## What is causing the crash?

**VkCreateShaderModule** with **shader-broken.spirv** file crashes with the following error:
```
Exception thrown at 0x00007FFBC2171722 (amdvlk64.dll) in main.exe: 0xC0000005: Access violation reading location 0x0000000000000008.
```

**shader-broken.spirv** is generated with my C -> SPIRV compiler [hcc](https://github.com/heroseh/hcc)

In **shader-broken.spirv**, the **OpVariable** with a **Output** storage class and a structure data type, is causing the crash. In **shader-works.spirv**, I removed these and their related instructions using a hex editor and it does not crash anymore.
```spirv
%5 = OpVariable %_ptr_Output__struct_52 Output
%40 = OpVariable %_ptr_Output__struct_39 Output
```

This does work when i use Linux on the same machine, so it is something specific to the AMD Vulkan driver for Windows.

I have not seen any other compiler (such as dxc or glslc) that uses structure types in **OpVariable** with **Output** or **Input** storage classes but the SPIR-V spec does support it.

An **OpVariable** with an **Input** storage class that uses a structure type does work fine, so it is specifically to do with the **Output** storage class.

## How to reproduce the crash

I have attached a 250 LOC program that simply sets up vulkan and calls **VkCreateShaderModule** twice, once for the **shader-works.spirv** file followed by **shader-broken.spirv** file

You can build the program using the **build.bat** script, it requires clang to be installed and in your PATH.

Thank you for looking into this :)