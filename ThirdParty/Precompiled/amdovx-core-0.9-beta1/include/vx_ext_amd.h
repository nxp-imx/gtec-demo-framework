/* 
Copyright (c) 2015 Advanced Micro Devices, Inc. All rights reserved.
 
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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#ifndef _VX_EXT_AMD_H_
#define _VX_EXT_AMD_H_

#include <VX/vx.h>
#ifdef __cplusplus
#include <string>
#endif

/*! \brief AMD target affinity enumerations for AgoTargetAffinityInfo.device_type
*/
#define AGO_TARGET_AFFINITY_CPU       0x0010 // CPU
#define AGO_TARGET_AFFINITY_GPU       0x0020 // GPU

/*! \brief AMD internal parameters. [TODO: This needs to be moved to ago_internal.h]
*/
#define AGO_MAX_PARAMS                                   32
#define AGO_MERGE_RULE_MAX_FIND                           4
#define AGO_MERGE_RULE_MAX_REPLACE                        4
#define AGO_MERGE_RULE_SOLITARY_FLAG                   0x20
#define AGO_TARGET_AFFINITY_GPU_INFO_DEVICE_MASK       0x0F
#define AGO_TARGET_AFFINITY_GPU_INFO_SVM_MASK          0xF0
#define AGO_TARGET_AFFINITY_GPU_INFO_SVM_ENABLE        0x10
#define AGO_TARGET_AFFINITY_GPU_INFO_SVM_AS_CLMEM      0x20
#define AGO_TARGET_AFFINITY_GPU_INFO_SVM_NO_FGS        0x40

/*! \brief Maximum size of scalar string buffer. The local buffers used for accessing scalar strings 
* should be of size VX_MAX_STRING_BUFFER_SIZE_AMD and the maximum allowed string length is
* VX_MAX_STRING_BUFFER_SIZE_AMD-1.
* \ingroup group_scalar
*/
#define VX_MAX_STRING_BUFFER_SIZE_AMD                   256

/*! \brief The type enumeration lists all the AMD specific types in OpenVX.
*/
enum ago_type_public_e {
	/*! \brief AMD data types
	*/
	VX_TYPE_STRING_AMD          = VX_TYPE_SCALAR_MAX,        // scalar data type for string

	/*! \brief AMD data structs
	*/
	AGO_TYPE_KEYPOINT_XYS = VX_TYPE_VENDOR_STRUCT_START,     // AGO struct data type for keypoint XYS

	/*! \brief AMD data object types
	*/
	AGO_TYPE_MEANSTDDEV_DATA = VX_TYPE_VENDOR_OBJECT_START,  // AGO data structure for AGO MeanStdDev kernels
	AGO_TYPE_MINMAXLOC_DATA,                                 // AGO data structure for AGO MinMaxLoc kernels
	AGO_TYPE_CANNY_STACK,                                    // AGO data structure for AGO Canny kernels
	AGO_TYPE_SCALE_MATRIX,                                   // AGO data structure for AGO Scale kernels
};

/*! \brief The AMD context attributes list.
*/
enum vx_context_attribute_amd_e {
	/*! \brief OpenCL context. Use a <tt>\ref cl_context</tt> parameter.*/
	VX_CONTEXT_ATTRIBUTE_AMD_OPENCL_CONTEXT = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_CONTEXT) + 0x01,
	/*! \brief context affinity. Use a <tt>\ref AgoTargetAffinityInfo</tt> parameter.*/
	VX_CONTEXT_ATTRIBUTE_AMD_AFFINITY       = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_CONTEXT) + 0x02,
	/*! \brief set a text macro definition. Use a <tt>\ref AgoContextMacroInfo</tt> parameter.*/
	VX_CONTEXT_ATTRIBUTE_AMD_SET_TEXT_MACRO = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_CONTEXT) + 0x03,
	/*! \brief set a merge rule. Use a <tt>\ref AgoNodeMergeRule</tt> parameter.*/
	VX_CONTEXT_ATTRIBUTE_AMD_SET_MERGE_RULE = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_CONTEXT) + 0x04,
};

/*! \brief The AMD kernel attributes list.
*/
enum vx_kernel_attribute_amd_e {
	/*! \brief kernel callback for query target support. Use a <tt>\ref amd_kernel_query_target_support_f</tt> parameter.*/
	VX_KERNEL_ATTRIBUTE_AMD_QUERY_TARGET_SUPPORT    = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_KERNEL) + 0x01,
	/*! \brief kernel callback for OpenCL code generation. Use a <tt>\ref amd_kernel_opencl_codegen_callback_f</tt> parameter.*/
	VX_KERNEL_ATTRIBUTE_AMD_OPENCL_CODEGEN_CALLBACK = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_KERNEL) + 0x02,
	/*! \brief kernel callback for node regeneration. Use a <tt>\ref amd_kernel_node_regen_callback_f</tt> parameter.*/
	VX_KERNEL_ATTRIBUTE_AMD_NODE_REGEN_CALLBACK     = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_KERNEL) + 0x03,
};

/*! \brief The AMD graph attributes list.
*/
enum vx_graph_attribute_amd_e {
	/*! \brief graph affinity. Use a <tt>\ref AgoNodeAffinityInfo</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_AFFINITY                     = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x01,
	/*! \brief imports a graph from a text file. Use a <tt>\ref AgoGraphImportInfo</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_IMPORT_FROM_TEXT             = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x02,
	/*! \brief export a graph into a text file. Use a <tt>\ref AgoGraphExportInfo</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_EXPORT_TO_TEXT               = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x03,
	/*! \brief graph optimizer flags. Use a <tt>\ref vx_uint32</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_OPTIMIZER_FLAGS              = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x04,
	/*! \brief graph last performance (internal). Use a <tt>\ref AgoGraphPerfInternalInfo</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_PERFORMANCE_INTERNAL_LAST    = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x05,
	/*! \brief graph avg performance (internal). Use a <tt>\ref AgoGraphPerfInternalInfo</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_PERFORMANCE_INTERNAL_AVG     = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x06,
	/*! \brief graph internal performance profile. Use a char * fileName parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_PERFORMANCE_INTERNAL_PROFILE = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x07,
	/*! \brief OpenCL command queue. Use a <tt>\ref cl_command_queue</tt> parameter.*/
	VX_GRAPH_ATTRIBUTE_AMD_OPENCL_COMMAND_QUEUE         = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_GRAPH) + 0x08,
};

/*! \brief The AMD node attributes list.
*/
enum vx_node_attribute_amd_e {
	/*! \brief node affinity. Use a <tt>\ref AgoTargetAffinityInfo</tt> parameter.*/
	VX_NODE_ATTRIBUTE_AMD_AFFINITY          = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_NODE) + 0x01,
};

/*! \brief The AMD image attributes list.
*/
enum vx_image_attribute_amd_e {
	/*! \brief sync with user specified OpenCL buffer. Use a <tt>\ref cl_mem</tt> parameter.*/
	VX_IMAGE_ATTRIBUTE_AMD_OPENCL_BUFFER             = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_IMAGE) + 0x01,
	/*! \brief sync with user specified OpenCL buffer offset. Use a <tt>\ref cl_uint</tt> parameter.*/
	VX_IMAGE_ATTRIBUTE_AMD_OPENCL_BUFFER_OFFSET      = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_IMAGE) + 0x02,
	/*! \brief Enable user kernel's own OpenCL buffer for virtual images. Supports only images with
	* single color plane and stride should match framework's internal alignment. image ROI not supported.
	* Use a <tt>\ref vx_bool</tt> parameter.*/
	VX_IMAGE_ATTRIBUTE_AMD_ENABLE_USER_BUFFER_OPENCL = VX_ATTRIBUTE_BASE(VX_ID_AMD, VX_TYPE_IMAGE) + 0x03,
};

/*! \brief These enumerations are given to the \c vxDirective API to enable/disable
* platform optimizations and/or features. Directives are not optional and
* usually are vendor-specific, by defining a vendor range of directives and
* starting their enumeration from there.
* \see <tt>vxDirective</tt>
* \ingroup group_directive
*/
enum vx_directive_amd_e {
	/*! \brief data object is readonly after this directive is given. */
	VX_DIRECTIVE_AMD_READ_ONLY      = VX_ENUM_BASE(VX_ID_AMD, VX_ENUM_DIRECTIVE) + 0x01,
	/*! \brief data object copy to OpenCL. */
	VX_DIRECTIVE_AMD_COPY_TO_OPENCL = VX_ENUM_BASE(VX_ID_AMD, VX_ENUM_DIRECTIVE) + 0x02,
};

/*! \brief Based on the VX_DF_IMAGE definition.
* \note Use <tt>\ref vx_df_image</tt> to contain these values.
*/
enum vx_df_image_amd_e {
	VX_DF_IMAGE_U1_AMD    = VX_DF_IMAGE('U', '0', '0', '1'),  // AGO image with 1-bit data
	VX_DF_IMAGE_F32_AMD   = VX_DF_IMAGE('F', '0', '3', '2'),  // AGO image with 32-bit floating-point (float)
	VX_DF_IMAGE_F64_AMD   = VX_DF_IMAGE('F', '0', '6', '4'),  // AGO image with 64-bit floating-point (double)
	VX_DF_IMAGE_F32x3_AMD = VX_DF_IMAGE('F', '3', '3', '2'),  // AGO image with THREE 32-bit floating-point channels in one buffer
};

/*! \brief module entry point for vxUnpublishKernels.
*/
typedef vx_status(VX_API_CALL *vx_unpublish_kernels_f)(vx_context context);

/*! \brief AMD data structure to specify target affinity.
*/
typedef struct {
	vx_uint32 device_type; // shall be AGO_TARGET_AFFINITY_CPU or AGO_TARGET_AFFINITY_GPU
	vx_uint32 device_info; // reserved -- shall be initialized to ZERO and shall not be modified
	vx_uint32 reserved[2]; // reserved -- shall be initialized to ZERO and shall not be modified
} AgoTargetAffinityInfo;

/*! \brief AMD data structure to set a text macro.
*/
typedef struct {
	vx_char macroName[256];
	vx_char * text;
} AgoContextTextMacroInfo;

/*! \brief AMD data structure to import a graph from a text.
**    text:
**      "macro <macro-name>" to use a pre-defined macro
**      "file <file-name>" to load from a file
**      otherwise use the text as is
*/
typedef struct {
	vx_char * text;
	vx_uint32 num_ref;
	vx_reference * ref;
	vx_int32 dumpToConsole;
	void (VX_CALLBACK * data_registry_callback_f) (void * obj, vx_reference ref, const char * name, const char * app_params);
	void * data_registry_callback_obj;
} AgoGraphImportInfo;

/*! \brief AMD data structure to export a graph to a text.
*/
typedef struct {
	vx_char fileName[256];
	vx_uint32 num_ref;
	vx_reference * ref;
	vx_char comment[64];
} AgoGraphExportInfo;

/*! \brief AMD data structure to get internal performance data.
*/
typedef struct {
	vx_uint64 kernel_enqueue;
	vx_uint64 kernel_wait;
	vx_uint64 buffer_read;
	vx_uint64 buffer_write;
} AgoGraphPerfInternalInfo;

/*! \brief AMD data structure to specify node merge rule.
*/
typedef struct AgoNodeMergeRule_t {
	struct {
		vx_enum    kernel_id;
		vx_uint32  arg_spec[AGO_MAX_PARAMS];
	} find[AGO_MERGE_RULE_MAX_FIND];
	struct {
		vx_enum    kernel_id;
		vx_uint32  arg_spec[AGO_MAX_PARAMS];
	} replace[AGO_MERGE_RULE_MAX_REPLACE];
} AgoNodeMergeRule;

#ifdef __cplusplus
/*! \brief AMD usernode callback for target support check - supported_target_affinity shall contain bitfields AGO_TARGET_AFFINITY_CPU and AGO_TARGET_AFFINITY_GPU.
*   When this callback is not available, the framework assumes that supported_target_affinity = AGO_TARGET_AFFINITY_CPU.
*/
typedef vx_status(VX_CALLBACK * amd_kernel_query_target_support_f) (vx_graph graph, vx_node node,
	vx_bool use_opencl_1_2,              // [input]  false: OpenCL driver is 2.0+; true: OpenCL driver is 1.2
	vx_uint32& supported_target_affinity // [output] must be set to AGO_TARGET_AFFINITY_CPU or AGO_TARGET_AFFINITY_GPU or (AGO_TARGET_AFFINITY_CPU | AGO_TARGET_AFFINITY_GPU)
	);

/*! \brief AMD usernode callback for OpenCL source code generation. The framework will pass
*   OpenVX objects as parameters to OpenCL kernels in othe order they appear to OpenVX node.
*   The mapping of OpenVX object to OpenCL kernel argument as shown below:
*     vx_image:       uint width, uint height, __global <type> * buf, uint stride_in_bytes, uint offset
*     vx_array:       __global <type> * buf, uint offset_in_bytes, uint numitems
*     vx_scalar:      float value or uint value or int value
*     vx_matrix:      float matrix[<ROWS>*<COLS>]
*     vx_convolution: float convolution[<ROWS>*<COLS>]
*     vx_threshold:   int value or int2 value
*     vx_remap:       __global short2 * buf, uint stride_in_bytes
*     vx_lut:         __read_only image1d_t lut
*/
typedef vx_status(VX_CALLBACK * amd_kernel_opencl_codegen_callback_f) (vx_node node,
	bool opencl_load_function,                     // [input]  false: normal OpenCL kernel; true: reserved
	char opencl_kernel_function_name[64],          // [output] kernel_name for clCreateKernel()
	std::string& opencl_kernel_code,               // [output] string for clCreateProgramWithSource()
	std::string& opencl_build_options,             // [output] options for clBuildProgram()
	vx_uint32& opencl_work_dim,                    // [output] work_dim for clEnqueueNDRangeKernel()
	vx_size opencl_global_work[],                  // [output] global_work[] for clEnqueueNDRangeKernel()
	vx_size opencl_local_work[],                   // [output] local_work[] for clEnqueueNDRangeKernel()
	vx_uint32& opencl_local_buffer_usage_mask,     // [output] reserved: must be ZERO
	vx_uint32& opencl_local_buffer_size_in_bytes   // [output] reserved: must be ZERO
	);

/*! \brief AMD usernode callback for regenerating a node.
*/
typedef vx_status(VX_CALLBACK * amd_kernel_node_regen_callback_f) (vx_graph graph, vx_node node, vx_bool& regen_not_needed);
#endif

/*==============================================================================
MISCELLANEOUS
=============================================================================*/

/*! \brief Name a reference
* \ingroup vx_framework_reference
*
* This function is used to associate a name to a reference. This name
* can be used by the OpenVX implementation in log messages and any
* other reporting mechanisms.
*
* The OpenVX implementation will not check if the name is unique in
* the reference scope (context or graph). Several references can then
* have the same name.
*
* \param [in] ref The reference to name.
* \param [in] name Pointer to the '\0' terminated string that identifies
*             the reference.
*             The string is copied by the function so that it
*             stays the property of the caller.
*             NULL means that the reference is not named.
* \return A \ref vx_status_e enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_ERROR_INVALID_REFERENCE if reference is not valid.
*/
VX_API_ENTRY vx_status VX_API_CALL vxSetReferenceName(vx_reference ref, const vx_char *name);

/**
* \brief Retrieve the name of a reference
* \ingroup vx_framework_reference
*
* This function is used to retrieve the name of a reference.
*
* \param [in] ref The reference.
* \param [out] name Pointer to copy the name of the reference.
* \param [in] size Size of the name buffer.
* \return A \ref vx_status_e enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_ERROR_INVALID_REFERENCE if reference is not valid.
*/
VX_API_ENTRY vx_status VX_API_CALL vxGetReferenceName(vx_reference ref, vx_char name[], vx_size size);

/**
* \brief Set module internal data.
* \ingroup vx_framework_reference
*
* This function is used to set module specific internal data. This is for use by vxPublishKernels().
*
* \param [in] context The context.
* \param [in] module The name of the module used in vxLoadKernels.
* \param [in] ptr The module internal buffer.
* \param [in] size Size of the module internal buffer.
* \return A \ref vx_status_e enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_ERROR_INVALID_REFERENCE if reference is not valid.
*/
VX_API_ENTRY vx_status VX_API_CALL vxSetModuleInternalData(vx_context context, const vx_char * module, void * ptr, vx_size size);

/**
* \brief Retrieve module internal data.
* \ingroup vx_framework_reference
*
* This function is used to retrieve module specific internal data. This is for use by vxUnpublishKernels().
*
* \param [in] context The context.
* \param [in] module The name of the module used in vxLoadKernels.
* \param [out] ptr The module internal buffer.
* \param [out] size Size of the module internal buffer.
* \return A \ref vx_status_e enumeration.
* \retval VX_SUCCESS No errors.
* \retval VX_ERROR_INVALID_REFERENCE if reference is not valid.
*/
VX_API_ENTRY vx_status VX_API_CALL vxGetModuleInternalData(vx_context context, const vx_char * module, void ** ptr, vx_size * size);

#endif
