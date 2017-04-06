/*
 * Copyright (c) 2012-2015 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef _OPENVX_API_H_
#define _OPENVX_API_H_

/*!
 * \file
 * \brief The API definition for OpenVX.
 */

#ifdef  __cplusplus
extern "C" {
#endif

/*==============================================================================
 CONTEXT
 =============================================================================*/

/*! \brief Creates a <tt>\ref vx_context</tt>.
 * \details This creates a top-level object context for OpenVX.
 * \note This is required to do anything else.
 * \returns The reference to the implementation context <tt>\ref vx_context</tt>. Any possible errors 
 * preventing a successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_context
 * \post <tt>\ref vxReleaseContext</tt>
 */
VX_API_ENTRY vx_context VX_API_CALL vxCreateContext();

/*! \brief Releases the OpenVX object context.
 * \details All reference counted objects are garbage-collected by the return of this call.
 * No calls are possible using the parameter context after the context has been
 * released until a new reference from <tt>\ref vxCreateContext</tt> is returned.
 * All outstanding references to OpenVX objects from this context are invalid
 * after this call.
 * \param [in] context The pointer to the reference to the context.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If context is not a <tt>\ref vx_context</tt>.
 * \ingroup group_context
 * \pre <tt>\ref vxCreateContext</tt>
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseContext(vx_context *context);

/*! \brief Retrieves the context from any reference from within a context.
 * \param [in] reference The reference from which to extract the context.
 * \ingroup group_context
 * \return The overall context that created the particular
 * reference.
 */
VX_API_ENTRY vx_context VX_API_CALL vxGetContext(vx_reference reference);

/*! \brief Queries the context for some specific information.
 * \param [in] context The reference to the context.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_context_attribute_e</tt>.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the context is not a <tt>\ref vx_context</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \retval VX_ERROR_NOT_SUPPORTED If the attribute is not supported on this implementation.
 * \ingroup group_context
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryContext(vx_context context, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Sets an attribute on the context.
 * \param [in] context The handle to the overall context.
 * \param [in] attribute The attribute to set from <tt>\ref vx_context_attribute_e</tt>.
 * \param [in] ptr The pointer to the data to which to set the attribute.
 * \param [in] size The size in bytes of the data to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the context is not a <tt>\ref vx_context</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \retval VX_ERROR_NOT_SUPPORTED If the attribute is not settable.
 * \ingroup group_context
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetContextAttribute(vx_context context, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Provides a generic API to give platform-specific hints to the implementation.
 * \param [in] reference The reference to the object to hint at.
 * This could be <tt>\ref vx_context</tt>, <tt>\ref vx_graph</tt>, <tt>\ref vx_node</tt>, <tt>\ref vx_image</tt>, <tt>\ref vx_array</tt>, or any other reference.
 * \param [in] hint A <tt>\ref vx_hint_e</tt> \a hint to give the OpenVX context. This is a platform-specific optimization or implementation mechanism.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No error.
 * \retval VX_ERROR_INVALID_REFERENCE If context or reference is invalid.
 * \retval VX_ERROR_NOT_SUPPORTED If the hint is not supported.
 * \ingroup group_hint
 */
VX_API_ENTRY vx_status VX_API_CALL vxHint(vx_reference reference, vx_enum hint);

/*! \brief Provides a generic API to give platform-specific directives to the implementations.
 * \param [in] reference The reference to the object to set the directive on.
 * This could be <tt>\ref vx_context</tt>, <tt>\ref vx_graph</tt>, <tt>\ref vx_node</tt>, <tt>\ref vx_image</tt>, <tt>\ref vx_array</tt>, or any other reference.
 * \param [in] directive The directive to set.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No error.
 * \retval VX_ERROR_INVALID_REFERENCE If context or reference is invalid.
 * \retval VX_ERROR_NOT_SUPPORTED If the directive is not supported.
 * \ingroup group_directive
 */
VX_API_ENTRY vx_status VX_API_CALL vxDirective(vx_reference reference, vx_enum directive);

/*! \brief Provides a generic API to return status values from Object constructors if they
 * fail.
 * \note Users do not need to strictly check every object creator as the errors
 * should properly propogate and be detected during verification time or run-time.
 * \code
 * vx_image img = vxCreateImage(context, 639, 480, VX_DF_IMAGE_UYVY);
 * vx_status status = vxGetStatus((vx_reference)img);
 * // status == VX_ERROR_INVALID_DIMENSIONS
 * vxReleaseImage(&img);
 * \endcode
 * \pre Appropriate Object Creator function.
 * \post Appropriate Object Release function.
 * \param [in] reference The reference to check for construction errors.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No error.
 * \retval * Some error occurred, please check enumeration list and constructor.
 * \ingroup group_basic_features
 */
VX_API_ENTRY vx_status VX_API_CALL vxGetStatus(vx_reference reference);

/*!
 * \brief Registers user-defined structures to the context.
 * \param [in] context  The reference to the implementation context.
 * \param [in] size     The size of user struct in bytes.
 * \return A <tt>\ref vx_enum</tt> value that is a type given to the User
 * to refer to their custom structure when declaring a <tt>\ref vx_array</tt>
 * of that structure.
 * \retval VX_TYPE_INVALID If the namespace of types has been exhausted.
 * \note This call should only be used once within the lifetime of a context for
 * a specific structure.
 *
 * \snippet vx_arrayrange.c array define
 * \ingroup group_adv_array
 */
VX_API_ENTRY vx_enum VX_API_CALL vxRegisterUserStruct(vx_context context, vx_size size);

/*==============================================================================
 IMAGE
 =============================================================================*/

/*! \brief Creates an opaque reference to an image buffer.
 * \details Not guaranteed to exist until the <tt>\ref vx_graph</tt> containing it has been verified.
 * \param [in] context The reference to the implementation context.
 * \param [in] width The image width in pixels.
 * \param [in] height The image height in pixels.
 * \param [in] color The VX_DF_IMAGE (<tt>\ref vx_df_image_e</tt>) code that represents the format of the image and the color space.
 * \returns An image reference <tt>\ref vx_image</tt>. Any possible errors preventing a successful
 * creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \see vxAccessImagePatch to obtain direct memory access to the image data.
 * \ingroup group_image
 */
VX_API_ENTRY vx_image VX_API_CALL vxCreateImage(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color);

/*! \brief Creates an image from another image given a rectangle. This second
 * reference refers to the data in the original image. Updates to this image
 * updates the parent image. The rectangle must be defined within the pixel space
 * of the parent image.
 * \param [in] img The reference to the parent image.
 * \param [in] rect The region of interest rectangle. Must contain points within
 * the parent image pixel space.
 * \returns An image reference <tt>\ref vx_image</tt> to the sub-image. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_image
 */
VX_API_ENTRY vx_image VX_API_CALL vxCreateImageFromROI(vx_image img, const vx_rectangle_t *rect);

/*! \brief Creates a reference to an image object that has a singular,
 * uniform value in all pixels.
 * \details The value pointer must reflect the specific format of the desired
 * image. For example:
 * | Color       | Value Ptr  |
 * |:------------|:-----------|
 * | <tt>\ref VX_DF_IMAGE_U8</tt>   | vx_uint8 * |
 * | <tt>\ref VX_DF_IMAGE_S16</tt>  | vx_int16 * |
 * | <tt>\ref VX_DF_IMAGE_U16</tt>  | vx_uint16 *|
 * | <tt>\ref VX_DF_IMAGE_S32</tt>  | vx_int32 * |
 * | <tt>\ref VX_DF_IMAGE_U32</tt>  | vx_uint32 *|
 * | <tt>\ref VX_DF_IMAGE_RGB</tt>  | vx_uint8 pixel[3] in R, G, B order |
 * | <tt>\ref VX_DF_IMAGE_RGBX</tt> | vx_uint8 pixels[4] |
 * | Any YUV     | vx_uint8 pixel[3] in Y, U, V order |
 *
 * \param [in] context The reference to the implementation context.
 * \param [in] width The image width in pixels.
 * \param [in] height The image height in pixels.
 * \param [in] color The VX_DF_IMAGE (\ref vx_df_image_e) code that represents the format of the image and the color space.
 * \param [in] value The pointer to the pixel value to which to set all pixels.
 * \returns An image reference <tt>\ref vx_image</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * <tt>\see vxAccessImagePatch</tt> to obtain direct memory access to the image data.
 * \note <tt>\ref vxAccessImagePatch</tt> and <tt>\ref vxCommitImagePatch</tt> may be called with
 * a uniform image reference.
 * \ingroup group_image
 */
VX_API_ENTRY vx_image VX_API_CALL vxCreateUniformImage(vx_context context, vx_uint32 width, vx_uint32 height, vx_df_image color, const void *value);

/*! \brief Creates an opaque reference to an image buffer with no direct
 * user access. This function allows setting the image width, height, or format.
 * \details Virtual data objects allow users to connect various nodes within a
 * graph via data references without access to that data, but they also permit the
 * implementation to take maximum advantage of possible optimizations. Use this
 * API to create a data reference to link two or more nodes together when the
 * intermediate data are not required to be accessed by outside entities. This API
 * in particular allows the user to define the image format of the data without
 * requiring the exact dimensions. Virtual objects are scoped within the graph
 * they are declared a part of, and can't be shared outside of this scope.
 * All of the following constructions of virtual images are valid.
 * \code
 * vx_context context = vxCreateContext();
 * vx_graph graph = vxCreateGraph(context);
 * vx_image virt[] = {
 *     vxCreateVirtualImage(graph, 0, 0, VX_DF_IMAGE_U8), // no specified dimension
 *     vxCreateVirtualImage(graph, 320, 240, VX_DF_IMAGE_VIRT), // no specified format
 *     vxCreateVirtualImage(graph, 640, 480, VX_DF_IMAGE_U8), // no user access
 * };
 * \endcode
 * \param [in] graph The reference to the parent graph.
 * \param [in] width The width of the image in pixels. A value of zero informs the interface that the value is unspecified.
 * \param [in] height The height of the image in pixels. A value of zero informs the interface that the value is unspecified.
 * \param [in] color The VX_DF_IMAGE (<tt>\ref vx_df_image_e</tt>) code that represents the format of the image and the color space. A value of <tt>\ref VX_DF_IMAGE_VIRT</tt> informs the interface that the format is unspecified.
 * \returns An image reference <tt>\ref vx_image</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \note Passing this reference to <tt>\ref vxAccessImagePatch</tt> will return an error.
 * \ingroup group_image
 */
VX_API_ENTRY vx_image VX_API_CALL vxCreateVirtualImage(vx_graph graph, vx_uint32 width, vx_uint32 height, vx_df_image color);

/*! \brief Creates a reference to an image object that was externally allocated.
 * \param [in] context The reference to the implementation context.
 * \param [in] color See the <tt>\ref vx_df_image_e</tt> codes. This mandates the
 * number of planes needed to be valid in the \a addrs and \a ptrs arrays based on the format given.
 * \param [in] addrs[] The array of image patch addressing structures that
 * define the dimension and stride of the array of pointers.
 * \param [in] ptrs[] The array of platform-defined references to each plane.
 * \param [in] import_type <tt>\ref vx_import_type_e</tt>. When giving <tt>\ref VX_IMPORT_TYPE_HOST</tt>
 * the \a ptrs array is assumed to be HOST accessible pointers to memory.
 * \returns An image reference <tt>\ref vx_image</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \note The user must call vxAccessImagePatch prior to accessing the pixels of an image, even if the 
 * image was created via <tt>\ref vxCreateImageFromHandle</tt>. Reads or writes to memory referenced 
 * by ptrs[ ] after calling <tt>\ref vxCreateImageFromHandle</tt> without first calling 
 * <tt>\ref vxAccessImagePatch</tt> will result in undefined behavior.
 * \ingroup group_image
 */
VX_API_ENTRY vx_image VX_API_CALL vxCreateImageFromHandle(vx_context context, vx_df_image color, vx_imagepatch_addressing_t addrs[], void *ptrs[], vx_enum import_type);

/*! \brief Retrieves various attributes of an image.
 * \param [in] image The reference to the image to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_image_attribute_e</tt>.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the image is not a <tt>\ref vx_image</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \retval VX_ERROR_NOT_SUPPORTED If the attribute is not supported on this implementation.
 * \ingroup group_image
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryImage(vx_image image, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Allows setting attributes on the image.
 * \param [in] image The reference to the image on which to set the attribute.
 * \param [in] attribute The attribute to set. Use a <tt>\ref vx_image_attribute_e</tt> enumeration.
 * \param [in] ptr The pointer to the location from which to read the value.
 * \param [in] size The size in bytes of the object pointed to by \a ptr.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the image is not a <tt>\ref vx_image</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \ingroup group_image
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetImageAttribute(vx_image image, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Releases a reference to an image object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] image The pointer to the image to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If image is not a <tt>\ref vx_image</tt>.
 * \ingroup group_image
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseImage(vx_image *image);

/*! \brief This computes the size needed to retrieve an image patch from an image.
 * \param [in] image The reference to the image from which to extract the patch.
 * \param [in] rect The coordinates. Must be 0 <= start < end <= dimension where
 * dimension is width for x and height for y.
 * \param [in] plane_index The plane index from which to get the data.
 * \return vx_size
 * \ingroup group_image
 */
VX_API_ENTRY vx_size VX_API_CALL vxComputeImagePatchSize(vx_image image,
                                       const vx_rectangle_t *rect,
                                       vx_uint32 plane_index);

/*! \brief Allows the User to extract a rectangular patch (subset) of an image from a single plane.
 * \param [in] image The reference to the image from which to extract the patch.
 * \param [in] rect The coordinates from which to get the patch. Must be 0 <= start < end.
 * \param [in] plane_index The plane index from which to get the data.
 * \param [in, out] addr A pointer to a structure describing the addressing information of the 
 * image patch to accessed.
 * \arg Input case: ptr is a pointer to a non-NULL pointer. The addr parameter must be the 
 * address of an addressing 
 * structure that describes how the user will access the requested image data at address (*ptr).
 * \arg Output case: ptr is a pointer to a NULL pointer. The function fills the structure pointed by 
 * addr with the 
 * addressing information that the user must consult to access the pixel data at address (*ptr). 
 * If the image being accessed was created via <tt>\ref vxCreateImageFromHandle</tt>, then the
 * returned addressing information will be the identical to that of the addressing structure provided 
 * when <tt>\ref vxCreateImageFromHandle</tt> was called.
 
 * \param [in, out] ptr A pointer to a pointer to a location to store the requested data.
 * \arg Input case: ptr is a pointer to a non-NULL pointer to a valid pixel buffer. This buffer 
 * will be used in one 
 * of two ways, depending on the value of the usage parameter. If usage is VX_WRITE_ONLY, then the 
 * buffer must contain pixel data that the user wants to replace the image's pixel data with.
 * Otherwise (i.e., usage is not VX_WRITE_ONLY), the image's current pixel data will be written to the 
 * memory starting at address (*ptr) as storage memory for the access request. The caller must ensure 
 * enough memory has been allocated for the requested patch with the requested addressing.
 * If image was created via <tt>\ref vxCreateImageFromHandle</tt>, and the pixel buffer pointed to by (*ptr) overlaps 
 * the original pixel buffer provided when image was created, then the results of such a call to 
 * <tt>\ref vxAccessImagePatch</tt> are undefined.
 * \arg Output case: ptr is a pointer to a NULL pointer. This NULL pointer will be overwritten 
 * with a pointer to the 
 * address where the requested data can be accessed. If image was created via 
 * <tt>\ref vxCreateImageFromHandle</tt> 
 * then the overwriting pointer must be within the original pixel buffer provided when image was created.
 * \arg (*ptr) must eventually be provided as the ptr parameter of a call to 
 * <tt>\ref vxCommitImagePatch</tt>.

 * \param [in] usage This declares the intended usage of the pointer using the <tt>\ref vx_accessor_e</tt> enumeration. For uniform images Only VX_READ_ONLY is supported.
 * \note The addr and ptr parameters must both be input, or both be output, otherwise the behavior is undefined.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_OPTIMIZED_AWAY The reference is a virtual image and cannot be accessed or committed.
 * \retval VX_ERROR_INVALID_PARAMETERS The \a start, \a end, \a plane_index, \a stride_x, or \a stride_y pointer is incorrect.
 * \retval VX_ERROR_INVALID_REFERENCE The image reference is not actually an image reference.
 * \note The user may ask for data outside the bounds of the valid region, but
 * such data has an undefined value.
 * \note Users must be cautious to prevent passing in \e uninitialized pointers or
 * addresses of uninitialized pointers to this function.
 * \pre <tt>\ref vxComputeImagePatchSize</tt> if users wish to allocate their own memory.
 * \post <tt>\ref vxCommitImagePatch</tt> with same (*ptr) value.
 * \ingroup group_image
 * \include vx_imagepatch.c
 */
VX_API_ENTRY vx_status VX_API_CALL vxAccessImagePatch(vx_image image,
                                    const vx_rectangle_t *rect,
                                    vx_uint32 plane_index,
                                    vx_imagepatch_addressing_t *addr,
                                    void **ptr,
                                    vx_enum usage);

/*! \brief This allows the User to commit a rectangular patch (subset) of an image from a single plane.
 * \param [in] image The reference to the image from which to extract the patch.
 * \param [in] rect The coordinates to which to set the patch. Must be 0 <= start <= end.
 * This may be 0 or a rectangle of zero area in order to indicate that the commit
 * must only decrement the reference count.
 * \param [in] plane_index The plane index to which to set the data.
 * \param [in] addr The addressing information for the image patch.
 * \param [in] ptr A pointer to a pixel buffer to be committed. If the user previously provided a 
 * pointer to this buffer to <tt>\ref vxAccessImagePatch</tt>, the buffer can be
 * freed or re-used after <tt>\ref vxCommitImagePatch</tt> completes. If the pointer was returned by 
 * <tt>\ref vxAccessImagePatch</tt>, reads or writes to the location pointed by ptr after 
 * <tt>\ref vxCommitImagePatch</tt> completes will result in undefined behavior.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_OPTIMIZED_AWAY The reference is a virtual image and cannot be accessed or committed.
 * \retval VX_ERROR_INVALID_PARAMETERS The \a start, \a end, \a plane_index, \a stride_x, or \a stride_y pointer is incorrect.
 * \retval VX_ERROR_INVALID_REFERENCE The image reference is not actually an image reference.
 * \ingroup group_image
 * \include vx_imagepatch.c
 * \note If the implementation gives the client a pointer from
 * <tt>\ref vxAccessImagePatch</tt> then implementation-specific behavior may occur.
 * If not, then a copy occurs from the users pointer to the internal data of the object.
 * \note If the rectangle intersects bounds of the current valid region, the
 * valid region grows to the union of the two rectangles as long as they occur
 * within the bounds of the original image dimensions.
 */
VX_API_ENTRY vx_status VX_API_CALL vxCommitImagePatch(vx_image image,
                                    vx_rectangle_t *rect,
                                    vx_uint32 plane_index,
                                    vx_imagepatch_addressing_t *addr,
                                    const void *ptr);

/*!
 * \brief Accesses a specific indexed pixel in an image patch.
 * \param [in] ptr The base pointer of the patch as returned from <tt>\ref vxAccessImagePatch</tt>.
 * \param [in] index The 0 based index of the pixel count in the patch. Indexes increase horizontally by 1 then wrap around to the next row.
 * \param [in] addr The pointer to the addressing mode information returned from <tt>\ref vxAccessImagePatch</tt>.
 * \return void * Returns the pointer to the specified pixel.
 * \pre <tt>\ref vxAccessImagePatch</tt>
 * \include vx_imagepatch.c
 * \ingroup group_image
 */
VX_API_ENTRY void * VX_API_CALL vxFormatImagePatchAddress1d(void *ptr, vx_uint32 index, const vx_imagepatch_addressing_t *addr);

/*!
 * \brief Accesses a specific pixel at a 2d coordinate in an image patch.
 * \param [in] ptr The base pointer of the patch as returned from <tt>\ref vxAccessImagePatch</tt>.
 * \param [in] x The x dimension within the patch.
 * \param [in] y The y dimension within the patch.
 * \param [in] addr The pointer to the addressing mode information returned from <tt>\ref vxAccessImagePatch</tt>.
 * \return void * Returns the pointer to the specified pixel.
 * \pre <tt>\ref vxAccessImagePatch</tt>
 * \include vx_imagepatch.c
 * \ingroup group_image
 */
VX_API_ENTRY void * VX_API_CALL vxFormatImagePatchAddress2d(void *ptr, vx_uint32 x, vx_uint32 y, const vx_imagepatch_addressing_t *addr);

/*! \brief Retrieves the valid region of the image as a rectangle.
 * \details After the image is allocated but has not been written to this
 * returns the full rectangle of the image so that functions do not have to manage
 * a case for uninitialized data. The image still retains an uninitialized
 * value, but once the image is written to via any means such as <tt>\ref vxCommitImagePatch</tt>,
 * the valid region is altered to contain the maximum bounds of the written
 * area.
 * \param [in] image The image from which to retrieve the valid region.
 * \param [out] rect The destination rectangle.
 * \return vx_status
 * \retval VX_ERROR_INVALID_REFERENCE Invalid image.
 * \retval VX_ERROR_INVALID_PARAMETERS Invalid rect.
 * \retval VX_SUCCESS Valid image.
 * \note This rectangle can be passed directly to <tt>\ref vxAccessImagePatch</tt> to get
 * the full valid region of the image. Modifications from <tt>\ref vxCommitImagePatch</tt>
 * grows the valid region.
 * \ingroup group_image
 */
VX_API_ENTRY vx_status VX_API_CALL vxGetValidRegionImage(vx_image image, vx_rectangle_t *rect);

/*==============================================================================
 KERNEL
 =============================================================================*/

/*! \brief Loads one or more kernels into the OpenVX context. This is the interface
 * by which OpenVX is extensible. Once the set of kernels is loaded new kernels
 * and their parameters can be queried.
 * \note When all references to loaded kernels are released, the module
 * may be automatically unloaded.
 * \param [in] context The reference to the implementation context.
 * \param [in] module The short name of the module to load. On systems where
 * there are specific naming conventions for modules, the name passed
 * should ignore such conventions. For example: \c libxyz.so should be
 * passed as just \c xyz and the implementation will <i>do the right thing</i> that
 * the platform requires.
 * \note This API uses the system pre-defined paths for modules.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the context is not a <tt>\ref vx_context</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \ingroup group_user_kernels
 * \see vxGetKernelByName
 */
VX_API_ENTRY vx_status VX_API_CALL vxLoadKernels(vx_context context, const vx_char *module);

/*! \brief Obtains a reference to a kernel using a string to specify the name.
 * \details User Kernels follow a "dotted" heirarchical syntax. For example:
 * "com.company.example.xyz". The following are strings specifying the kernel names:

 * org.khronos.openvx.color_convert

 * org.khronos.openvx.channel_extract

 * org.khronos.openvx.channel_combine

 * org.khronos.openvx.sobel_3x3

 * org.khronos.openvx.magnitude

 * org.khronos.openvx.phase

 * org.khronos.openvx.scale_image

 * org.khronos.openvx.table_lookup

 * org.khronos.openvx.histogram

 * org.khronos.openvx.equalize_histogram

 * org.khronos.openvx.absdiff

 * org.khronos.openvx.mean_stddev

 * org.khronos.openvx.threshold

 * org.khronos.openvx.integral_image

 * org.khronos.openvx.dilate_3x3

 * org.khronos.openvx.erode_3x3

 * org.khronos.openvx.median_3x3

 * org.khronos.openvx.box_3x3

 * org.khronos.openvx.gaussian_3x3

 * org.khronos.openvx.custom_convolution

 * org.khronos.openvx.gaussian_pyramid

 * org.khronos.openvx.accumulate

 * org.khronos.openvx.accumulate_weighted

 * org.khronos.openvx.accumulate_square

 * org.khronos.openvx.minmaxloc

 * org.khronos.openvx.convertdepth

 * org.khronos.openvx.canny_edge_detector

 * org.khronos.openvx.and

 * org.khronos.openvx.or

 * org.khronos.openvx.xor

 * org.khronos.openvx.not

 * org.khronos.openvx.multiply

 * org.khronos.openvx.add

 * org.khronos.openvx.subtract

 * org.khronos.openvx.warp_affine

 * org.khronos.openvx.warp_perspective

 * org.khronos.openvx.harris_corners

 * org.khronos.openvx.fast_corners

 * org.khronos.openvx.optical_flow_pyr_lk

 * org.khronos.openvx.remap

 * org.khronos.openvx.halfscale_gaussian 

 * \param [in] context The reference to the implementation context.
 * \param [in] name The string of the name of the kernel to get.
 * \return A kernel reference or zero if an error occurred.
 * \retval 0 The kernel name is not found in the context.
 * \ingroup group_kernel
 * \pre <tt>\ref vxLoadKernels</tt> if the kernel is not provided by the
 * OpenVX implementation.
 * \note User Kernels should follow a "dotted" heirarchical syntax. For example:
 * "com.company.example.xyz".
 */
VX_API_ENTRY vx_kernel VX_API_CALL vxGetKernelByName(vx_context context, const vx_char *name);

/*! \brief Obtains a reference to the kernel using the <tt>\ref vx_kernel_e</tt> enumeration.
 * \details Enum values above the standard set are assumed to apply to
 * loaded libraries.
 * \param [in] context The reference to the implementation context.
 * \param [in] kernel A value from <tt>\ref vx_kernel_e</tt> or a vendor or client-defined value.
 * \return A <tt>\ref vx_kernel</tt>.
 * \retval 0 The kernel enumeration is not found in the context.
 * \ingroup group_kernel
 * \pre <tt>\ref vxLoadKernels</tt> if the kernel is not provided by the
 * OpenVX implementation.
 */
VX_API_ENTRY vx_kernel VX_API_CALL vxGetKernelByEnum(vx_context context, vx_enum kernel);

/*! \brief This allows the client to query the kernel to get information about
 * the number of parameters, enum values, etc.
 * \param [in] kernel The kernel reference to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_kernel_attribute_e</tt>.
 * \param [out] ptr The pointer to the location at which to store the resulting value.
 * \param [in] size The size of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If the kernel is not a <tt>\ref vx_kernel</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS If any of the other parameters are incorrect.
 * \retval VX_ERROR_NOT_SUPPORTED If the attribute value is not supported in this implementation.
 * \ingroup group_kernel
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryKernel(vx_kernel kernel, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Release the reference to the kernel.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] kernel The pointer to the kernel reference to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If kernel is not a <tt>\ref vx_kernel</tt>.
 * \ingroup group_kernel
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseKernel(vx_kernel *kernel);

/*! \brief Allows users to add custom kernels to the known kernel
 * database in OpenVX at run-time. This would primarily be used by the module function
 * \c vxPublishKernels.
 * \param [in] context The reference to the implementation context.
 * \param [in] name The string to use to match the kernel.
 * \param [in] enumeration The enumerated value of the kernel to be used by clients.
 * \param [in] func_ptr The process-local function pointer to be invoked.
 * \param [in] numParams The number of parameters for this kernel.
 * \param [in] input The pointer to <tt>\ref vx_kernel_input_validate_f</tt>, which validates the
 * input parameters to this kernel.
 * \param [in] output The pointer to <tt>\ref vx_kernel_output_validate_f </tt>, which validates the
 * output parameters to this kernel.
 * \param [in] init The kernel initialization function.
 * \param [in] deinit The kernel de-initialization function.
 * \ingroup group_user_kernels
 * \return <tt>\ref vx_kernel</tt>
 * \retval 0 Indicates that an error occurred when adding the kernel.
 * \retval * Kernel added to OpenVX.
 */
VX_API_ENTRY vx_kernel VX_API_CALL vxAddKernel(vx_context context,
                             const vx_char name[VX_MAX_KERNEL_NAME],
                             vx_enum enumeration,
                             vx_kernel_f func_ptr,
                             vx_uint32 numParams,
                             vx_kernel_input_validate_f input,
                             vx_kernel_output_validate_f output,
                             vx_kernel_initialize_f init,
                             vx_kernel_deinitialize_f deinit);

/*! \brief This API is called after all parameters have been added to the
 * kernel and the kernel is \e ready to be used. Notice that the reference to the kernel created 
 * by vxAddKernel is still valid after the call to vxFinalizeKernel.
 * \param [in] kernel The reference to the loaded kernel from <tt>\ref vxAddKernel</tt>.
 * \return A <tt>\ref vx_status_e</tt> enumeration. If an error occurs, the kernel is not available
 * for usage by the clients of OpenVX. Typically this is due to a mismatch
 * between the number of parameters requested and given.
 * \pre <tt>\ref vxAddKernel</tt> and <tt>\ref vxAddParameterToKernel</tt>
 * \ingroup group_user_kernels
 */
VX_API_ENTRY vx_status VX_API_CALL vxFinalizeKernel(vx_kernel kernel);

/*! \brief Allows users to set the signatures of the custom kernel.
 * \param [in] kernel The reference to the kernel added with <tt>\ref vxAddKernel</tt>.
 * \param [in] index The index of the parameter to add.
 * \param [in] dir The direction of the parameter. This must be either <tt>\ref VX_INPUT</tt> or 
 * <tt>\ref VX_OUTPUT</tt>. <tt>\ref VX_BIDIRECTIONAL</tt> is not supported for this function. 
 * \param [in] data_type The type of parameter. This must be a value from <tt>\ref vx_type_e</tt>.
 * \param [in] state The state of the parameter (required or not). This must be a value from <tt>\ref vx_parameter_state_e</tt>.
 * \return A <tt>\ref vx_status_e</tt> enumerated value.
 * \retval VX_SUCCESS Parameter is successfully set on kernel.
 * \retval VX_ERROR_INVALID_REFERENCE The value passed as kernel was not a \c vx_kernel.
 * \pre <tt>\ref vxAddKernel</tt>
 * \ingroup group_user_kernels
 */
VX_API_ENTRY vx_status VX_API_CALL vxAddParameterToKernel(vx_kernel kernel, vx_uint32 index, vx_enum dir, vx_enum data_type, vx_enum state);

/*! \brief Removes a non-finalized <tt>\ref vx_kernel</tt> from the <tt>\ref vx_context</tt> 
 * and releases it. Once a <tt>\ref vx_kernel</tt> has been finalized it cannot be removed.
 * \param [in] kernel The reference to the kernel to remove. Returned from <tt>\ref vxAddKernel</tt>.
 * \note Any kernel enumerated in the base standard
 * cannot be removed; only kernels added through <tt>\ref vxAddKernel</tt> can
 * be removed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_INVALID_REFERENCE If an invalid kernel is passed in.
 * \retval VX_ERROR_INVALID_PARAMETER If a base kernel is passed in.
 * \ingroup group_user_kernels
 */
VX_API_ENTRY vx_status VX_API_CALL vxRemoveKernel(vx_kernel kernel);

/*! \brief Sets kernel attributes.
 * \param [in] kernel The reference to the kernel.
 * \param [in] attribute The enumeration of the attributes. See <tt>\ref vx_kernel_attribute_e</tt>.
 * \param [in] ptr The pointer to the location from which to read the attribute.
 * \param [in] size The size in bytes of the data area indicated by \a ptr in bytes.
 * \note After a kernel has been passed to <tt>\ref vxFinalizeKernel</tt>, no attributes
 * can be altered.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_user_kernels
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetKernelAttribute(vx_kernel kernel, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Retrieves a <tt>\ref vx_parameter</tt> from a <tt>\ref vx_kernel</tt>.
 * \param [in] kernel The reference to the kernel.
 * \param [in] index The index of the parameter.
 * \return A <tt>\ref vx_parameter</tt>.
 * \retval 0 Either the kernel or index is invalid.
 * \retval * The parameter reference.
 * \ingroup group_parameter
 */
VX_API_ENTRY vx_parameter VX_API_CALL vxGetKernelParameterByIndex(vx_kernel kernel, vx_uint32 index);

/*==============================================================================
 GRAPH
 =============================================================================*/

/*! \brief Creates an empty graph.
 * \param [in] context The reference to the implementation context.
 * \returns A graph reference <tt>\ref vx_graph</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_graph VX_API_CALL vxCreateGraph(vx_context context);

/*! \brief Releases a reference to a graph.
 * The object may not be garbage collected until its total reference count is zero.
 * Once the reference count is zero, all node references in the graph are automatically
 * released as well. Data referenced by those nodes may not be released as
 * the user may have external references to the data.
 * \param [in] graph The pointer to the graph to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If graph is not a <tt>\ref vx_graph</tt>.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseGraph(vx_graph *graph);

/*! \brief Verifies the state of the graph before it is executed.
 * This is useful to catch programmer errors and contract errors. If not verified,
 * the graph verifies before being processed.
 * \pre Memory for data objects is not guarenteed to exist before
 * this call. \post After this call data objects exist unless
 * the implementation optimized them out.
 * \param [in] graph The reference to the graph to verify.
 * \return A status code for graphs with more than one error; it is
 * undefined which error will be returned. Register a log callback using <tt>\ref vxRegisterLogCallback</tt>
 * to receive each specific error in the graph.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If graph is not a <tt>\ref vx_graph</tt>.
 * \retval VX_ERROR_MULTIPLE_WRITERS If the graph contains more than one writer
 * to any data object.
 * \retval VX_ERROR_INVALID_NODE If a node in the graph is invalid or failed be created.
 * \retval VX_ERROR_INVALID_GRAPH If the graph contains cycles or some other invalid topology.
 * \retval VX_ERROR_INVALID_TYPE If any parameter on a node is given the wrong type.
 * \retval VX_ERROR_INVALID_VALUE If any value of any parameter is out of bounds of specification.
 * \retval VX_ERROR_INVALID_FORMAT If the image format is not compatible.
 * \ingroup group_graph
 * \see vxProcessGraph
 */
VX_API_ENTRY vx_status VX_API_CALL vxVerifyGraph(vx_graph graph);

/*! \brief This function causes the synchronous processing of a graph. If the graph
 * has not been verified, then the implementation verifies the graph
 * immediately. If verification fails this function returns a status
 * identical to what <tt>\ref vxVerifyGraph</tt> would return. After
 * the graph verfies successfully then processing occurs. If the graph was
 * previously verified via <tt>\ref vxVerifyGraph</tt> or <tt>\ref vxProcessGraph</tt>
 * then the graph is processed. This function blocks until the graph is completed.
 * \param [in] graph The graph to execute.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Graph has been processed.
 * \retval VX_FAILURE A catastrophic error occurred during processing.
 * \retval * See <tt>\ref vxVerifyGraph</tt>.
 * \pre <tt>\ref vxVerifyGraph</tt> must return <tt>\ref VX_SUCCESS</tt> before this function will pass.
 * \ingroup group_graph
 * \see vxVerifyGraph
 */
VX_API_ENTRY vx_status VX_API_CALL vxProcessGraph(vx_graph graph);

/*! \brief Schedules a graph for future execution.
 * \param [in] graph The graph to schedule.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_NO_RESOURCES The graph cannot be scheduled now.
 * \retval VX_ERROR_NOT_SUFFICIENT The graph is not verified and has failed
forced verification.
 * \retval VX_SUCCESS The graph has been scheduled.
 * \pre <tt>\ref vxVerifyGraph</tt> must return <tt>\ref VX_SUCCESS</tt> before this function will pass.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_status VX_API_CALL vxScheduleGraph(vx_graph graph);

/*! \brief Waits for a specific graph to complete. If the graph has been scheduled multiple 
 * times since the last call to vxWaitGraph, then vxWaitGraph returns only when the last 
 * scheduled execution completes.
 * \param [in] graph The graph to wait on.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS The graph has successfully completed execution and its outputs are the 
 * valid results of the most recent execution. 
 * \retval VX_FAILURE An error occurred or the graph was never scheduled.  Use \ref vxQueryGraph 
 * for the VX_GRAPH_ATTRIBUTE_STATUS attribute to determine the details.  Output data of the 
 * graph is undefined. 
 * \pre <tt>\ref vxScheduleGraph</tt>
 * \ingroup group_graph
 */
VX_API_ENTRY vx_status VX_API_CALL vxWaitGraph(vx_graph graph);

/*! \brief Allows the user to query attributes of the Graph.
 * \param [in] graph The reference to the created graph.
 * \param [in] attribute The <tt>\ref vx_graph_attribute_e</tt> type needed.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryGraph(vx_graph graph, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Allows the set to attributes on the Graph.
 * \param [in] graph The reference to the graph.
 * \param [in] attribute The <tt>\ref vx_graph_attribute_e</tt> type needed.
 * \param [in] ptr The location from which to read the value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetGraphAttribute(vx_graph graph, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Adds the given parameter extracted from a <tt>\ref vx_node</tt> to the graph.
 * \param [in] graph The graph reference that contains the node.
 * \param [in] parameter The parameter reference to add to the graph from the node.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Parameter added to Graph.
 * \retval VX_ERROR_INVALID_REFERENCE The parameter is not a valid <tt>\ref vx_parameter</tt>.
 * \retval VX_ERROR_INVALID_PARAMETER The parameter is of a node not in this
 * graph.
 * \ingroup group_graph_parameters
 */
VX_API_ENTRY vx_status VX_API_CALL vxAddParameterToGraph(vx_graph graph, vx_parameter parameter);

/*! \brief Sets a reference to the parameter on the graph. The implementation
 * must set this parameter on the originating node as well.
 * \param [in] graph The graph reference.
 * \param [in] index The parameter index.
 * \param [in] value The reference to set to the parameter.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Parameter set to Graph.
 * \retval VX_ERROR_INVALID_REFERENCE The value is not a valid <tt>\ref vx_reference</tt>.
 * \retval VX_ERROR_INVALID_PARAMETER The parameter index is out of bounds or the
 * dir parameter is incorrect.
 * \ingroup group_graph_parameters
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetGraphParameterByIndex(vx_graph graph, vx_uint32 index, vx_reference value);

/*! \brief Retrieves a <tt>\ref vx_parameter</tt> from a <tt>\ref vx_graph</tt>.
 * \param [in] graph The graph.
 * \param [in] index The index of the parameter.
 * \return <tt>\ref vx_parameter</tt> reference.
 * \retval 0 if the index is out of bounds.
 * \retval * The parameter reference.
 * \ingroup group_graph_parameters
 */
VX_API_ENTRY vx_parameter VX_API_CALL vxGetGraphParameterByIndex(vx_graph graph, vx_uint32 index);

/*! \brief Returns a Boolean to indicate the state of graph verification.
 * \param [in] graph The reference to the graph to check.
 * \return A <tt>\ref vx_bool</tt> value.
 * \retval vx_true_e The graph is verified.
 * \retval vx_false_e The graph is not verified. It must be verified before
 * execution either through <tt>\ref vxVerifyGraph</tt> or automatically through
 * <tt>\ref vxProcessGraph</tt> or <tt>\ref vxScheduleGraph</tt>.
 * \ingroup group_graph
 */
VX_API_ENTRY vx_bool VX_API_CALL vxIsGraphVerified(vx_graph graph);

/*==============================================================================
 NODE
 =============================================================================*/

/*! \brief Creates a reference to a node object for a given kernel.
 * \details This node has no references assigned as parameters after completion.
 * The client is then required to set these parameters manually by <tt>\ref vxSetParameterByIndex</tt>.
 * When clients supply their own node creation functions (for use with User Kernels), this is the API
 * to use along with the parameter setting API.
 * \param [in] graph The reference to the graph in which this node exists.
 * \param [in] kernel The kernel reference to associate with this new node.
 * \returns A node reference <tt>\ref vx_node</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_adv_node
 * \post Call <tt>\ref vxSetParameterByIndex</tt> for as many parameters as needed to be set.
 */
VX_API_ENTRY vx_node VX_API_CALL vxCreateGenericNode(vx_graph graph, vx_kernel kernel);

/*! \brief Allows a user to query information out of a node.
 * \param [in] node The reference to the node to query.
 * \param [in] attribute Use <tt>\ref vx_node_attribute_e</tt> value to query for information.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytesin bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Successful
 * \retval VX_ERROR_INVALID_PARAMETERS The type or size is incorrect.
 * \ingroup group_node
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryNode(vx_node node, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Allows a user to set attribute of a node before Graph Validation.
 * \param [in] node The reference to the node to set.
 * \param [in] attribute Use <tt>\ref vx_node_attribute_e</tt> value to query for information.
 * \param [out] ptr The output pointer to where to send the value.
 * \param [in] size The size in bytes of the objects to which \a ptr points.
 * \note Some attributes are inherited from the <tt>\ref vx_kernel</tt>, which was used
 * to create the node. Some of these can be overridden using this API, notably
 * \ref VX_NODE_ATTRIBUTE_LOCAL_DATA_SIZE and \ref VX_NODE_ATTRIBUTE_LOCAL_DATA_PTR.
 * \ingroup group_node
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS The attribute was set.
 * \retval VX_ERROR_INVALID_REFERENCE node is not a vx_node.
 * \retval VX_ERROR_INVALID_PARAMETER size is not correct for the type needed.
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetNodeAttribute(vx_node node, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Releases a reference to a Node object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] node The pointer to the reference of the node to release.
 * \ingroup group_node
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If node is not a <tt>\ref vx_node</tt>.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseNode(vx_node *node);

/*! \brief Removes a Node from its parent Graph and releases it.
 * \param [in] node The pointer to the node to remove and release.
 * \ingroup group_node
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If node is not a <tt>\ref vx_node</tt>.
 */
VX_API_ENTRY vx_status VX_API_CALL vxRemoveNode(vx_node *node);

/*! \brief Assigns a callback to a node.
 * If a callback already exists in this node, this function must return an error
 * and the user may clear the callback by passing a NULL pointer as the callback.
 * \param [in] node The reference to the node.
 * \param [in] callback The callback to associate with completion of this
 * specific node.
 * \warning This must be used with <b><i>extreme</i></b> caution as it can \e ruin
 * optimizations in the power/performance efficiency of a graph.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Callback assigned.
 * \retval VX_ERROR_INVALID_REFERENCE The value passed as node was not a <tt>\ref vx_node</tt>.
 * \ingroup group_node_callback
 */
VX_API_ENTRY vx_status VX_API_CALL vxAssignNodeCallback(vx_node node, vx_nodecomplete_f callback);

/*! \brief Retrieves the current node callback function pointer set on the node.
 * \param [in] node The reference to the <tt>\ref vx_node</tt> object.
 * \ingroup group_node_callback
 * \return vx_nodecomplete_f The pointer to the callback function.
 * \retval NULL No callback is set.
 * \retval * The node callback function.
 */
VX_API_ENTRY vx_nodecomplete_f VX_API_CALL vxRetrieveNodeCallback(vx_node node);

/*==============================================================================
 PARAMETER
 =============================================================================*/

/*! \brief Retrieves a <tt>\ref vx_parameter</tt> from a <tt>\ref vx_node</tt>.
 * \param [in] node The node from which to extract the parameter.
 * \param [in] index The index of the parameter to which to get a reference.
 * \return <tt>\ref vx_parameter</tt>
 * \ingroup group_parameter
 */
VX_API_ENTRY vx_parameter VX_API_CALL vxGetParameterByIndex(vx_node node, vx_uint32 index);

/*! \brief Releases a reference to a parameter object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] param The pointer to the parameter to release.
 * \ingroup group_parameter
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If param is not a <tt>\ref vx_parameter</tt>.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseParameter(vx_parameter *param);

/*! \brief Sets the specified parameter data for a kernel on the node.
 * \param [in] node The node that contains the kernel.
 * \param [in] index The index of the parameter desired.
 * \param [in] value The reference to the parameter.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_parameter
 * \see vxSetParameterByReference
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetParameterByIndex(vx_node node, vx_uint32 index, vx_reference value);

/*! \brief Associates a parameter reference and a data reference with a kernel
 * on a node.
 * \param [in] parameter The reference to the kernel parameter.
 * \param [in] value The value to associate with the kernel parameter.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_parameter
 * \see vxGetParameterByIndex
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetParameterByReference(vx_parameter parameter, vx_reference value);

/*! \brief Allows the client to query a parameter to determine its meta-information.
 * \param [in] param The reference to the parameter.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_parameter_attribute_e</tt>.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_parameter
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryParameter(vx_parameter param, vx_enum attribute, void *ptr, vx_size size);

/*==============================================================================
 SCALAR
 =============================================================================*/

/*! \brief Creates a reference to a scalar object. Also see \ref sub_node_parameters.
 * \param [in] context The reference to the system context.
 * \param [in] data_type The <tt>\ref vx_type_e</tt> of the scalar. Must be greater than
 * <tt>\ref VX_TYPE_INVALID</tt> and less than <tt>\ref VX_TYPE_SCALAR_MAX</tt>.
 * \param [in] ptr The pointer to the initial value of the scalar.
 * \ingroup group_scalar
 * \returns A scaler reference <tt>\ref vx_scalar</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 */
VX_API_ENTRY vx_scalar VX_API_CALL vxCreateScalar(vx_context context, vx_enum data_type, const void *ptr);

/*! \brief Releases a reference to a scalar object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] scalar The pointer to the scalar to release.
 * \ingroup group_scalar
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If scalar is not a <tt>\ref vx_scalar</tt>.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseScalar(vx_scalar *scalar);

/*! \brief Queries attributes from a scalar.
 * \param [in] scalar The scalar object.
 * \param [in] attribute The enumeration to query. Use a <tt>\ref vx_scalar_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_scalar
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryScalar(vx_scalar scalar, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Gets the scalar value out of a reference.
 * \note Use this in conjunction with Query APIs that return references which
 * should be converted into values.
 * \ingroup group_scalar
 * \param [in] ref The reference from which to get the scalar value.
 * \param [out] ptr An appropriate typed pointer that points to a location to which to copy
 * the scalar value.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_INVALID_REFERENCE If the ref is not a valid
 * reference.
 * \retval VX_ERROR_INVALID_PARAMETERS If \a ptr is NULL.
 * \retval VX_ERROR_INVALID_TYPE If the type does not match the type in the reference or is a bad value.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReadScalarValue(vx_scalar ref, void *ptr);

/*! \brief Sets the scalar value in a reference.
 * \note Use this in conjunction with Parameter APIs that return references
 * to parameters that need to be altered.
 * \ingroup group_scalar
 * \param [in] ref The reference from which to get the scalar value.
 * \param [in] ptr An appropriately typed pointer that points to a location to which to copy
 * the scalar value.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_ERROR_INVALID_REFERENCE If the ref is not a valid
 * reference.
 * \retval VX_ERROR_INVALID_PARAMETERS If \a ptr is NULL.
 * \retval VX_ERROR_INVALID_TYPE If the type does not match the type in the reference or is a bad value.
 */
VX_API_ENTRY vx_status VX_API_CALL vxWriteScalarValue(vx_scalar ref, const void *ptr);

/*==============================================================================
 REFERENCE
 =============================================================================*/

/*! \brief Queries any reference type for some basic information (count, type).
 * \param [in] ref The reference to query.
 * \param [in] attribute The value for which to query. Use <tt>\ref vx_reference_attribute_e</tt>.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_reference
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryReference(vx_reference ref, vx_enum attribute, void *ptr, vx_size size);

/*==============================================================================
 DELAY
 =============================================================================*/

/*! \brief Queries a <tt>\ref vx_delay</tt> object attribute.
 * \param [in] delay A pointer to a delay object.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_delay_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_delay
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryDelay(vx_delay delay, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Releases a reference to a delay object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] delay The pointer to the delay to release.
 * \post After returning from this function the reference is zeroed.
 * \ingroup group_delay
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If delay is not a <tt>\ref vx_delay</tt>.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseDelay(vx_delay *delay);

/*! \brief Creates a Delay object.
 * \details This function uses a subset of the attributes defining the metadata of 
 * the exemplar, ignoring the object. It does not alter the exemplar or keep or release 
 * the reference to the exemplar. For the definition of supported attributes
 * see \ref vxSetMetaFormatAttribute.
 * 
 * \param [in] context The reference to the system context.
 * \param [in] exemplar The exemplar object.
 * \param [in] slots The number of reference in the delay.
 * \returns A delay reference <tt>\ref vx_delay</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_delay
 */
VX_API_ENTRY vx_delay VX_API_CALL vxCreateDelay(vx_context context,
                              vx_reference exemplar,
                              vx_size slots);

/*! \brief Retrieves a reference from a delay object.
 * \param [in] delay The reference to the delay object.
 * \param [in] index An index into the delay from which to extract the
 * reference.
 * \return <tt>\ref vx_reference</tt>
 * \note The delay index is in the range \f$ [-count+1,0] \f$. 0 is always the
 * \e current object.
 * \ingroup group_delay
 * \note A reference from a delay object must not be given to its associated
 * release API (e.g. <tt>\ref vxReleaseImage</tt>). Use the <tt>\ref vxReleaseDelay</tt> only.
 */
VX_API_ENTRY vx_reference VX_API_CALL vxGetReferenceFromDelay(vx_delay delay, vx_int32 index);

/*! \brief Ages the internal delay ring by one. This means that once this API is
 * called the reference from index 0 will go to index -1 and so forth until
 * \f$ -count+1 \f$ is reached. This last object will become 0. Once the delay has
 * been aged, it updates the reference in any associated nodes.
 * \param [in] delay
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS Delay was aged.
 * \retval VX_ERROR_INVALID_REFERENCE The value passed as delay was not a <tt>\ref vx_delay</tt>.
 * \ingroup group_delay
 */
VX_API_ENTRY vx_status VX_API_CALL vxAgeDelay(vx_delay delay);


/*==============================================================================
 LOGGING
 =============================================================================*/

/*! \brief Adds a line to the log.
 * \param [in] ref The reference to add the log entry against. Some valid value must be provided.
 * \param [in] status The status code. <tt>\ref VX_SUCCESS</tt> status entries are ignored and not added.
 * \param [in] message The human readable message to add to the log.
 * \param [in] ... a list of variable arguments to the message.
 * \note Messages may not exceed <tt>\ref VX_MAX_LOG_MESSAGE_LEN</tt> bytes and will be truncated in the log if they exceed this limit.
 * \ingroup group_log
 */
VX_API_ENTRY void VX_API_CALL vxAddLogEntry(vx_reference ref, vx_status status, const char *message, ...);

/*! \brief Registers a callback facility to the OpenVX implementation to receive error logs.
 * \param [in] context The overall context to OpenVX.
 * \param [in] callback The callback function. If NULL, the previous callback is removed.
 * \param [in] reentrant If reentrancy flag is <tt>\ref vx_true_e</tt>, then the callback may be entered from multiple
 * simultaneous tasks or threads (if the host OS supports this).
 * \ingroup group_log
 */
VX_API_ENTRY void VX_API_CALL vxRegisterLogCallback(vx_context context, vx_log_callback_f callback, vx_bool reentrant);

/*==============================================================================
 LUT
 =============================================================================*/

/*! \brief Creates LUT object of a given type.
 * \param [in] context The reference to the context.
 * \param [in] data_type The type of data stored in the LUT.
 * \param [in] count The number of entries desired.
 * \if OPENVX_STRICT_1_0
 * \note For OpenVX 1.0, count must be equal to 256 and data_type can only be \ref VX_TYPE_UINT8.
 * \endif
 * \returns An LUT reference <tt>\ref vx_lut</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_lut
 */
VX_API_ENTRY vx_lut VX_API_CALL vxCreateLUT(vx_context context, vx_enum data_type, vx_size count);

/*! \brief Releases a reference to a LUT object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] lut The pointer to the LUT to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If lut is not a <tt>\ref vx_lut</tt>.
 * \ingroup group_lut
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseLUT(vx_lut *lut);

/*! \brief Queries attributes from a LUT.
 * \param [in] lut The LUT to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_lut_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_lut
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryLUT(vx_lut lut, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Grants access to a LUT table and increments the object reference count in case of success.
 * \details There are several variations of call methodology:
 * \arg If \a ptr is NULL (which means the current data of the LUT is not desired),
 * the LUT reference count is incremented.
 * \arg If \a ptr is not NULL but (*ptr) is NULL, (*ptr) will contain the address of the LUT data when the function returns and
 * the reference count will be incremented. Whether the (*ptr) address is mapped
 * or allocated is undefined. (*ptr) must be returned to <tt>\ref vxCommitLUT</tt>.
 * \arg If \a ptr is not NULL and (*ptr) is not NULL, the user is signalling the implementation to copy the LUT data into the location specified
 * by (*ptr). Users must use <tt>\ref vxQueryLUT</tt> with <tt>\ref VX_LUT_ATTRIBUTE_SIZE</tt> to
 * determine how much memory to allocate for the LUT data.
 *
 * In any case, <tt>\ref vxCommitLUT</tt> must be called after LUT access is complete.
 * \param [in] lut The LUT from which to get the data.
 * \param [in,out] ptr ptr The user-supplied address to a pointer, via which the requested contents 
 * are returned.
 * \arg If ptr is NULL, an error occurs.
 * \arg If ptr is not NULL and (*ptr) is NULL, (*ptr) will be set to the address of a memory area 
 * managed by the OpenVX framework containing the requested data.
 * \arg If both ptr and (*ptr) are not NULL, requested data will be copied to (*ptr) (optionally in 
 * case of write-only access).
 * \param [in] usage This declares the intended usage of the pointer using the * <tt>\ref vx_accessor_e</tt> enumeration.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \post <tt>\ref vxCommitLUT</tt>
 * \ingroup group_lut
 */
VX_API_ENTRY vx_status VX_API_CALL vxAccessLUT(vx_lut lut, void **ptr, vx_enum usage);

/*! \brief Commits the Lookup Table and decrements the object reference count in case of success.
 * \details Commits the data back to the LUT object and decrements the reference count.
 * There are several variations of call methodology:
 * \arg If a user should allocated their own memory for the LUT data copy, the user is
 * obligated to free this memory.
 * \arg If \a ptr is not NULL and the (*ptr) for <tt>\ref vxAccessLUT</tt> was NULL,
 * it is undefined whether the implementation will unmap or copy and free the memory.
 * \param [in] lut The LUT to modify.
 * \param [in] ptr The pointer provided or returned by <tt>\ref vxAccessLUT</tt>. This cannot be NULL.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \pre <tt>\ref vxAccessLUT</tt>.
 * \ingroup group_lut
 */
VX_API_ENTRY vx_status VX_API_CALL vxCommitLUT(vx_lut lut, const void *ptr);

/*==============================================================================
 DISTRIBUTION
 =============================================================================*/

/*! \brief Creates a reference to a 1D Distribution of a consecutive interval [offset, offset + range - 1] 
 * defined by a start offset and valid range, divided equally into numBins parts.
 * \param [in] context The reference to the overall context.
 * \param [in] numBins The number of bins in the distribution.
 * \param [in] offset The start offset into the range value that marks the begining of the 1D Distribution.
 * \param [in] range The total number of the values.
 * \returns A distribution reference <tt>\ref vx_distribution</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_distribution
 */
VX_API_ENTRY vx_distribution VX_API_CALL vxCreateDistribution(vx_context context, vx_size numBins, vx_int32 offset, vx_uint32 range);

/*! \brief Releases a reference to a distribution object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] distribution The reference to the distribution to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If distribution is not a <tt>\ref vx_distribution</tt>.
 * \ingroup group_distribution
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseDistribution(vx_distribution *distribution);

/*! \brief Queries a Distribution object.
 * \param [in] distribution The reference to the distribution to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_distribution_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_distribution
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryDistribution(vx_distribution distribution, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Grants access to a distribution object and increments the object reference count in 
 * case of success.
 * \param [in] distribution The reference to the distribution to access.
 * \param [in, out] ptr The user-supplied address to a pointer, via which the requested contents 
 * are returned.
 * \arg If ptr is NULL, an error occurs.
 * \arg If ptr is not NULL and (*ptr) is NULL, (*ptr) will be set to the address of a memory area 
 * managed by the OpenVX framework containing the requested data.
 * \arg If both ptr and (*ptr) are not NULL, requested data will be copied to (*ptr) (optionally in 
 * case of write-only access).
 * \param [in] usage The <tt>\ref vx_accessor_e</tt> value to describe the access of the object.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \post <tt>\ref vxCommitDistribution</tt>
 * \ingroup group_distribution
 */
VX_API_ENTRY vx_status VX_API_CALL vxAccessDistribution(vx_distribution distribution, void **ptr, vx_enum usage);

/*! \brief Commits the distribution objec> and decrements the object reference count in case of success. 
 * The memory must be a vx_uint32 array of a value at least as big as the value returned via 
 * <tt>\ref VX_DISTRIBUTION_ATTRIBUTE_BINS</tt>.
 * \param [in] distribution The Distribution to modify.
 * \param [in] ptr The pointer provided or returned by <tt>\ref vxAccessDistribution</tt>. The ptr cannot
 * be NULL.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \pre <tt>\ref vxAccessDistribution</tt>.
 * \ingroup group_distribution
 */
VX_API_ENTRY vx_status VX_API_CALL vxCommitDistribution(vx_distribution distribution, const void * ptr);

/*==============================================================================
 THRESHOLD
 =============================================================================*/

/*! \brief Creates a reference to a threshold object of a given type.
 * \param [in] c The reference to the overall context.
 * \param [in] thresh_type The type of threshold to create.
 * \param [in] data_type The data type of the threshold's value(s).
 * \if OPENVX_STRICT_1_0
 * \note For OpenVX 1.0, data_type can only be <tt>\ref VX_TYPE_UINT8</tt>.
 * \endif
 * \returns An threshold reference <tt>\ref vx_threshold</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_threshold
 */
VX_API_ENTRY vx_threshold VX_API_CALL vxCreateThreshold(vx_context c, vx_enum thresh_type, vx_enum data_type);

/*! \brief Releases a reference to a threshold object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] thresh The pointer to the threshold to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If thresh is not a <tt>\ref vx_threshold</tt>.
 * \ingroup group_threshold
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseThreshold(vx_threshold *thresh);

/*! \brief Sets attributes on the threshold object.
 * \param [in] thresh The threshold object to set.
 * \param [in] attribute The attribute to modify. Use a <tt>\ref vx_threshold_attribute_e</tt> enumeration.
 * \param [in] ptr The pointer to the value to which to set the attribute.
 * \param [in] size The size of the data pointed to by \a ptr.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_threshold
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetThresholdAttribute(vx_threshold thresh, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Queries an attribute on the threshold object.
 * \param [in] thresh The threshold object to set.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_threshold_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_threshold
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryThreshold(vx_threshold thresh, vx_enum attribute, void *ptr, vx_size size);

/*==============================================================================
 MATRIX
 =============================================================================*/

/*! \brief Creates a reference to a matrix object.
 * \param [in] c The reference to the overall context.
 * \param [in] data_type The unit format of the matrix. <tt>\ref VX_TYPE_INT32</tt> or <tt>\ref VX_TYPE_FLOAT32</tt>.
 * \param [in] columns The first dimensionality.
 * \param [in] rows The second dimensionality.
 * \returns An matrix reference <tt>\ref vx_matrix</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_matrix
 */
VX_API_ENTRY vx_matrix VX_API_CALL vxCreateMatrix(vx_context c, vx_enum data_type, vx_size columns, vx_size rows);

/*! \brief Releases a reference to a matrix object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] mat The matrix reference to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If mat is not a <tt>\ref vx_matrix</tt>.
 * \ingroup group_matrix
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseMatrix(vx_matrix *mat);

/*! \brief Queries an attribute on the matrix object.
 * \param [in] mat The matrix object to set.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_matrix_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_matrix
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryMatrix(vx_matrix mat, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Gets the matrix data (copy).
 * \param [in] mat The reference to the matrix.
 * \param [out] array The array in which to place the matrix.
 * \see vxQueryMatrix and <tt>\ref VX_MATRIX_ATTRIBUTE_COLUMNS</tt> and <tt>\ref VX_MATRIX_ATTRIBUTE_ROWS</tt>
 * to get the needed number of elements of the array.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_matrix
 */
VX_API_ENTRY vx_status VX_API_CALL vxReadMatrix(vx_matrix mat, void *array);

/*! \brief Sets the matrix data (copy)
 * \param [in] mat The reference to the matrix.
 * \param [in] array The array containing the matrix to be written.
 * \see vxQueryMatrix and <tt>\ref VX_MATRIX_ATTRIBUTE_COLUMNS</tt> and <tt>\ref VX_MATRIX_ATTRIBUTE_ROWS</tt>
 * to get the needed number of elements of the array.'
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_matrix
 */
VX_API_ENTRY vx_status VX_API_CALL vxWriteMatrix(vx_matrix mat, const void *array);

/*==============================================================================
 CONVOLUTION
 =============================================================================*/

/*! \brief Creates a reference to a convolution matrix object.
 * \param [in] context The reference to the overall context.
 * \param [in] columns The columns dimension of the convolution.
 * Must be odd and greater than or equal to 3 and less than the value returned
 * from <tt>\ref VX_CONTEXT_ATTRIBUTE_CONVOLUTION_MAXIMUM_DIMENSION</tt>.
 * \param [in] rows The rows dimension of the convolution.
 * Must be odd and greater than or equal to 3 and less than the value returned
 * from <tt>\ref VX_CONTEXT_ATTRIBUTE_CONVOLUTION_MAXIMUM_DIMENSION</tt>.
 * \returns A convolution reference <tt>\ref vx_convolution</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_convolution VX_API_CALL vxCreateConvolution(vx_context context, vx_size columns, vx_size rows);

/*! \brief Releases the reference to a convolution matrix.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] conv The pointer to the convolution matrix to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If conv is not a <tt>\ref vx_convolution</tt>.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseConvolution(vx_convolution *conv);

/*! \brief Queries an attribute on the convolution matrix object.
 * \param [in] conv The convolution matrix object to set.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_convolution_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryConvolution(vx_convolution conv, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Sets attributes on the convolution object.
 * \param [in] conv The coordinates object to set.
 * \param [in] attribute The attribute to modify. Use a <tt>\ref vx_convolution_attribute_e</tt> enumeration.
 * \param [in] ptr The pointer to the value to which to set the attribute.
 * \param [in] size The size in bytes of the data pointed to by \a ptr.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetConvolutionAttribute(vx_convolution conv, vx_enum attribute, const void *ptr, vx_size size);

/*! \brief Gets the convolution data (copy).
 * \param [in] conv The reference to the convolution.
 * \param [out] array The array to place the convolution.
 * \see vxQueryConvolution and <tt>\ref VX_CONVOLUTION_ATTRIBUTE_SIZE</tt> to get the
 * needed number of bytes of the array.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_status VX_API_CALL vxReadConvolutionCoefficients(vx_convolution conv, vx_int16 *array);

/*! \brief Sets the convolution data (copy)
 * \param [in] conv The reference to the convolution.
 * \param [in] array The array containing the convolution to be written.
 * \see <tt>\ref vxQueryConvolution</tt> and <tt>\ref VX_CONVOLUTION_ATTRIBUTE_SIZE</tt> to get the
 * needed number of bytes of the array.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_convolution
 */
VX_API_ENTRY vx_status VX_API_CALL vxWriteConvolutionCoefficients(vx_convolution conv, const vx_int16 *array);

/*==============================================================================
 PYRAMID
 =============================================================================*/

/*! \brief Creates a reference to a pyramid object of the supplied number of levels.
 * \param [in] context The reference to the overall context.
 * \param [in] levels The number of levels desired. This is required to be a non-zero value.
 * \param [in] scale Used to indicate the scale between pyramid levels. This is required to be a non-zero positive value.
 * \if OPENVX_STRICT_1_0
 * In OpenVX 1.0, the only permissible values are <tt>\ref VX_SCALE_PYRAMID_HALF</tt> or <tt>\ref VX_SCALE_PYRAMID_ORB</tt>.
 * \endif
 * \param [in] width The width of the 0th level image in pixels.
 * \param [in] height The height of the 0th level image in pixels.
 * \param [in] format The format of all images in the pyramid. NV12, NV21, IYUV, UYVY and YUYV formats are not supported.
 * \returns A pyramid reference <tt>\ref vx_pyramid</tt> to the sub-image. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \ingroup group_pyramid
 */
VX_API_ENTRY vx_pyramid VX_API_CALL vxCreatePyramid(vx_context context, vx_size levels, vx_float32 scale, vx_uint32 width, vx_uint32 height, vx_df_image format);

/*! \brief Creates a reference to a virtual pyramid object of the supplied number of levels.
 * \details Virtual Pyramids can be used to connect Nodes together when the contents of the pyramids will
 * not be accessed by the user of the API.
 * All of the following constructions are valid:
 * \code
 * vx_context context = vxCreateContext();
 * vx_graph graph = vxCreateGraph(context);
 * vx_pyramid virt[] = {
 *     vxCreateVirtualPyramid(graph, 4, VX_SCALE_PYRAMID_HALF, 0, 0, VX_DF_IMAGE_VIRT), // no dimension and format specified for level 0
 *     vxCreateVirtualPyramid(graph, 4, VX_SCALE_PYRAMID_HALF, 640, 480, VX_DF_IMAGE_VIRT), // no format specified.
 *     vxCreateVirtualPyramid(graph, 4, VX_SCALE_PYRAMID_HALF, 640, 480, VX_DF_IMAGE_U8), // no access
 * };
 * \endcode
 * \param [in] graph The reference to the parent graph.
 * \param [in] levels The number of levels desired. This is required to be a non-zero value.
 * \param [in] scale Used to indicate the scale between pyramid levels. This is required to be a non-zero positive value.
 * \if OPENVX_STRICT_1_0
 * In OpenVX 1.0, the only permissible values are <tt>\ref VX_SCALE_PYRAMID_HALF</tt> or <tt>\ref VX_SCALE_PYRAMID_ORB</tt>.
 * \endif
 * \param [in] width The width of the 0th level image in pixels. This may be set to zero to indicate to the interface that the value is unspecified.
 * \param [in] height The height of the 0th level image in pixels. This may be set to zero to indicate to the interface that the value is unspecified.
 * \param [in] format The format of all images in the pyramid. This may be set to <tt>\ref VX_DF_IMAGE_VIRT</tt> to indicate that the format is unspecified.
 * \returns A pyramid reference <tt>\ref vx_pyramid</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 * \note Images extracted with <tt>\ref vxGetPyramidLevel</tt> behave as Virtual Images and
 * cause <tt>\ref vxAccessImagePatch</tt> to return errors.
 * \ingroup group_pyramid
 */
VX_API_ENTRY vx_pyramid VX_API_CALL vxCreateVirtualPyramid(vx_graph graph, vx_size levels, vx_float32 scale, vx_uint32 width, vx_uint32 height, vx_df_image format);


/*! \brief Releases a reference to a pyramid object.
 * The object may not be garbage collected until its total reference count is zero.
 * \param [in] pyr The pointer to the pyramid to release.
 * \ingroup group_pyramid
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If pyr is not a <tt>\ref vx_pyramid</tt>.
 * \post After returning from this function the reference is zeroed.
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleasePyramid(vx_pyramid *pyr);

/*! \brief Queries an attribute from an image pyramid.
 * \param [in] pyr The pyramid to query.
 * \param [in] attribute The attribute for which to query. Use a <tt>\ref vx_pyramid_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_pyramid
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryPyramid(vx_pyramid pyr, vx_enum attribute, void *ptr, vx_size size);

/*! \brief Retrieves a level of the pyramid as a <tt>\ref vx_image</tt>, which can be used
 * elsewhere in OpenVX. A call to vxReleaseImage is necessary to release an image for each 
 * call of vxGetPyramidLevel.
 * \param [in] pyr The pyramid object.
 * \param [in] index The index of the level, such that index is less than levels.
 * \return A <tt>\ref vx_image</tt> reference.
 * \retval 0 Indicates that the index or the object is invalid.
 * \ingroup group_pyramid
 */
VX_API_ENTRY vx_image VX_API_CALL vxGetPyramidLevel(vx_pyramid pyr, vx_uint32 index);

/*==============================================================================
 REMAP
 =============================================================================*/

/*! \brief Creates a remap table object.
 * \param [in] context The reference to the overall context.
 * \param [in] src_width Width of the source image in pixel.
 * \param [in] src_height Height of the source image in pixels.
 * \param [in] dst_width Width of the destination image in pixels.
 * \param [in] dst_height Height of the destination image in pixels.
 * \ingroup group_remap
 * \returns A remap reference <tt>\ref vx_remap</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 */
VX_API_ENTRY vx_remap VX_API_CALL vxCreateRemap(vx_context context,
                              vx_uint32 src_width,
                              vx_uint32 src_height,
                              vx_uint32 dst_width,
                              vx_uint32 dst_height);

/*! \brief Releases a reference to a remap table object. The object may not be
 * garbage collected until its total reference count is zero.
 * \param [in] table The pointer to the remap table to release.
 * \post After returning from this function the reference is zeroed.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If table is not a <tt>\ref vx_remap</tt>.
 * \ingroup group_remap
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseRemap(vx_remap *table);

/*! \brief Assigns a destination pixel mapping to the source pixel.
 * \param [in] table The remap table reference.
 * \param [in] dst_x The destination x coordinate.
 * \param [in] dst_y The destination y coordinate.
 * \param [in] src_x The source x coordinate in float representation to allow interpolation.
 * \param [in] src_y The source y coordinate in float representation to allow interpolation.
 * \ingroup group_remap
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetRemapPoint(vx_remap table,
                                 vx_uint32 dst_x, vx_uint32 dst_y,
                                 vx_float32 src_x, vx_float32 src_y);

/*! \brief Retrieves the source pixel point from a destination pixel.
 * \param [in] table The remap table reference.
 * \param [in] dst_x The destination x coordinate.
 * \param [in] dst_y The destination y coordinate.
 * \param [out] src_x The pointer to the location to store the source x coordinate in float representation to allow interpolation.
 * \param [out] src_y The pointer to the location to store the source y coordinate in float representation to allow interpolation.
 * \ingroup group_remap
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 */
VX_API_ENTRY vx_status VX_API_CALL vxGetRemapPoint(vx_remap table,
                                 vx_uint32 dst_x, vx_uint32 dst_y,
                                 vx_float32 *src_x, vx_float32 *src_y);

/*! \brief Queries attributes from a Remap table.
 * \param [in] r The remap to query.
 * \param [in] attribute The attribute to query. Use a <tt>\ref vx_remap_attribute_e</tt> enumeration.
 * \param [out] ptr The location at which to store the resulting value.
 * \param [in] size The size in bytes of the container to which \a ptr points.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \ingroup group_remap
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryRemap(vx_remap r, vx_enum attribute, void *ptr, vx_size size);

/*==============================================================================
 ARRAY
 =============================================================================*/

/*!
 * \brief Creates a reference to an Array object.
 *
 * User must specify the Array capacity (i.e., the maximal number of items that the array can hold).
 *
 * \param [in] context      The reference to the overall Context.
 * \param [in] item_type    The type of objects to hold. Use:
 *                          \arg <tt>\ref VX_TYPE_RECTANGLE</tt> for <tt>\ref vx_rectangle_t</tt>.
 *                          \arg <tt>\ref VX_TYPE_KEYPOINT</tt> for <tt>\ref vx_keypoint_t</tt>.
 *                          \arg <tt>\ref VX_TYPE_COORDINATES2D</tt> for <tt>\ref vx_coordinates2d_t</tt>.
 *                          \arg <tt>\ref VX_TYPE_COORDINATES3D</tt> for <tt>\ref vx_coordinates3d_t</tt>.
 *                          \arg <tt>\ref vx_enum</tt> Returned from <tt>\ref vxRegisterUserStruct</tt>.
 * \param [in] capacity     The maximal number of items that the array can hold.
 *
 * \returns An array reference <tt>\ref vx_array</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_array VX_API_CALL vxCreateArray(vx_context context, vx_enum item_type, vx_size capacity);

/*!
 * \brief Creates an opaque reference to a virtual Array with no direct user access.
 *
 * Virtual Arrays are useful when item type or capacity are unknown ahead of time
 * and the Array is used as internal graph edge. Virtual arrays are scoped within the parent graph only.
 *
 * All of the following constructions are allowed.
 * \code
 * vx_context context = vxCreateContext();
 * vx_graph graph = vxCreateGraph(context);
 * vx_array virt[] = {
 *     vxCreateVirtualArray(graph, 0, 0), // totally unspecified
 *     vxCreateVirtualArray(graph, VX_TYPE_KEYPOINT, 0), // unspecified capacity
 *     vxCreateVirtualArray(graph, VX_TYPE_KEYPOINT, 1000), // no access
 * };
 * \endcode
 *
 * \param [in] graph        The reference to the parent graph.
 * \param [in] item_type    The type of objects to hold.
 *                          This may to set to zero to indicate an unspecified item type.
 * \param [in] capacity     The maximal number of items that the array can hold.
 *                          This may be to set to zero to indicate an unspecified capacity.
 * \see vxCreateArray for a type list.
 * \returns A array reference <tt>\ref vx_array</tt>. Any possible errors preventing a 
 * successful creation should be checked using <tt>\ref vxGetStatus</tt>.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_array VX_API_CALL vxCreateVirtualArray(vx_graph graph, vx_enum item_type, vx_size capacity);

/*!
 * \brief Releases a reference of an Array object.
 * The object may not be garbage collected until its total reference count is zero.
 * After returning from this function the reference is zeroed.
 * \param [in] arr          The pointer to the Array to release.
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS No errors.
 * \retval VX_ERROR_INVALID_REFERENCE If arr is not a <tt>\ref vx_array</tt>.
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxReleaseArray(vx_array *arr);

/*!
 * \brief Queries the Array for some specific information.
 *
 * \param [in] arr          The reference to the Array.
 * \param [in] attribute    The attribute to query. Use a <tt>\ref vx_array_attribute_e</tt>.
 * \param [out] ptr         The location at which to store the resulting value.
 * \param [in] size         The size in bytes of the container to which \a ptr points.
 *
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS                   No errors.
 * \retval VX_ERROR_INVALID_REFERENCE   If the \a arr is not a <tt>\ref vx_array</tt>.
 * \retval VX_ERROR_NOT_SUPPORTED       If the \a attribute is not a value supported on this implementation.
 * \retval VX_ERROR_INVALID_PARAMETERS  If any of the other parameters are incorrect.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxQueryArray(vx_array arr, vx_enum attribute, void *ptr, vx_size size);

/*!
 * \brief Adds items to the Array.
 *
 * This function increases the container size.
 *
 * By default, the function does not reallocate memory,
 * so if the container is already full (number of elements is equal to capacity)
 * or it doesn't have enough space,
 * the function returns <tt>\ref VX_FAILURE</tt> error code.
 *
 * \param [in] arr          The reference to the Array.
 * \param [in] count        The total number of elements to insert.
 * \param [in] ptr          The location at which to store the input values.
 * \param [in] stride       The number of bytes between the beginning of two consecutive elements.
 *
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS                   No errors.
 * \retval VX_ERROR_INVALID_REFERENCE   If the \a arr is not a <tt>\ref vx_array</tt>.
 * \retval VX_FAILURE                   If the Array is full.
 * \retval VX_ERROR_INVALID_PARAMETERS  If any of the other parameters are incorrect.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxAddArrayItems(vx_array arr, vx_size count, const void *ptr, vx_size stride);

/*!
 * \brief Truncates an Array (remove items from the end).
 *
 * \param [in,out] arr          The reference to the Array.
 * \param [in] new_num_items    The new number of items for the Array.
 *
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS                   No errors.
 * \retval VX_ERROR_INVALID_REFERENCE   If the \a arr is not a <tt>\ref vx_array</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS  The \a new_size is greater than the current size.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxTruncateArray(vx_array arr, vx_size new_num_items);

/*!
 * \brief Grants access to a sub-range of an Array. The number of elements in the sub-range is given by (end - start).
 *
 * \param [in] arr          The reference to the Array.
 * \param [in] start        The start index.
 * \param [in] end          The end index. (end - start) elements are accessed from start.
 * \param [in, out] stride  A pointer to 'number of bytes' between the beginning of two consequent 
 * elements. 
 * \arg Input case: ptr is a pointer to a non-NULL pointer. The stride parameter must be the address 
 * of a vx_size scalar that describes how the user will access the requested array data at address 
 * (*ptr).
 * \arg Output Case: ptr is a pointer to a NULL pointer. The function fills the vx_size scalar 
 * pointed by stride with the element stride information that the user must consult to access the 
 * array elements at address (*ptr).
 * \param [out] ptr        A pointer to a pointer to a location to store the requested data.
 * \arg Input Case: ptr is a pointer to a non-NULL pointer to a valid buffer. This buffer will be 
 * used in one of two ways, depending on the value of the usage parameter. If usage is 
 * VX_WRITE_ONLY, then the buffer must contain element data that the user wants to replace the 
 * array's element data with. Otherwise (i.e., usage is not VX_WRITE_ONLY), the array's current 
 * element data will be written to the memory starting at address (*ptr) as storage memory for the 
 * access request. The caller must ensure enough memory has been allocated for the requested array 
 * range with the requested stride.
 * \arg Output Case: ptr is a pointer to a NULL pointer.  This NULL pointer will be overwritten with 
 * a pointer to the address where the requested data can be accessed. (*ptr) must eventually be provided 
 * as the ptr parameter of a call to vxCommitArrayRange. 
 * \param [in] usage        This declares the intended usage of the pointer using the <tt>\ref vx_accessor_e</tt> enumeration.
 *
 * \note The stride and ptr parameters must both be input, or both be output, otherwise the behavior 
 * is undefined.
 *
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS                   No errors.
 * \retval VX_ERROR_OPTIMIZED_AWAY      If the reference is a virtual array and cannot be accessed or committed.
 * \retval VX_ERROR_INVALID_REFERENCE   If the \a arr is not a <tt>\ref vx_array</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS  If any of the other parameters are incorrect.
 * \post <tt>\ref vxCommitArrayRange</tt>
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxAccessArrayRange(vx_array arr, vx_size start, vx_size end, vx_size *stride, void **ptr, vx_enum usage);

/*!
 * \brief Commits data back to the Array object.
 *
 * \details This allows a user to commit data to a sub-range of an Array. The number of elements in the sub-range is given by (end - start).
 *
 * \param [in] arr          The reference to the Array.
 * \param [in] start        The start index.
 * \param [in] end          The end index. (end - start) elements are accessed from start.
 * \param [in] ptr          The user supplied pointer.
 *
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS                   No errors.
 * \retval VX_ERROR_OPTIMIZED_AWAY      If the reference is a virtual array and cannot be accessed or committed.
 * \retval VX_ERROR_INVALID_REFERENCE   If the \a arr is not a <tt>\ref vx_array</tt>.
 * \retval VX_ERROR_INVALID_PARAMETERS  If any of the other parameters are incorrect.
 *
 * \ingroup group_array
 */
VX_API_ENTRY vx_status VX_API_CALL vxCommitArrayRange(vx_array arr, vx_size start, vx_size end, const void *ptr);

/*!
 * \brief Accesses a specific indexed element in an array.
 * \param [in] ptr The base pointer for the array range.
 * \param [in] index The index of the element, not byte, to access.
 * \param [in] stride The 'number of bytes' between the beginning of two consecutive elements. 
 * \ingroup group_array
 */
#define vxFormatArrayPointer(ptr, index, stride) \
    (&(((vx_uint8*)(ptr))[(index) * (stride)]))

/*!
 * \brief Allows access to an array item as a typecast pointer deference.
 * \param [in] type The type of the item to access.
 * \param [in] ptr The base pointer for the array range.
 * \param [in] index The index of the element, not byte, to access.
 * \param [in] stride The 'number of bytes' between the beginning of two consecutive elements. 
 * \ingroup group_array
 */
#define vxArrayItem(type, ptr, index, stride) \
    (*(type *)(vxFormatArrayPointer((ptr), (index), (stride))))

/*==============================================================================
 META FORMAT
 =============================================================================*/

/*! \brief This function allows a user to set the attributes of a <tt>\ref vx_meta_format</tt> object in a kernel output validator.
 * 
 * The \ref vx_meta_format object contains two types of information : data object meta data and 
 * some specific information that defines how the valid region of an image changes
 *
 * The meta data attributes that can be set are identified by this list:
 * - \ref vx_image : \ref VX_IMAGE_ATTRIBUTE_FORMAT, \ref VX_IMAGE_ATTRIBUTE_HEIGHT, \ref VX_IMAGE_ATTRIBUTE_WIDTH
 * - \ref vx_array : \ref VX_ARRAY_ATTRIBUTE_CAPACITY, \ref VX_ARRAY_ATTRIBUTE_ITEMTYPE
 * - \ref vx_pyramid : \ref VX_PYRAMID_ATTRIBUTE_FORMAT, \ref VX_PYRAMID_ATTRIBUTE_HEIGHT, \ref VX_PYRAMID_ATTRIBUTE_WIDTH, \ref VX_PYRAMID_ATTRIBUTE_LEVELS, \ref VX_PYRAMID_ATTRIBUTE_SCALE
 * - \ref vx_scalar : \ref VX_SCALAR_ATTRIBUTE_TYPE
 * - \ref vx_matrix : \ref VX_MATRIX_ATTRIBUTE_TYPE, \ref VX_MATRIX_ATTRIBUTE_ROWS, \ref VX_MATRIX_ATTRIBUTE_COLUMNS
 * - \ref vx_distribution : \ref VX_DISTRIBUTION_ATTRIBUTE_BINS, \ref VX_DISTRIBUTION_ATTRIBUTE_OFFSET, \ref VX_DISTRIBUTION_ATTRIBUTE_RANGE
 * - \ref vx_remap : \ref VX_REMAP_ATTRIBUTE_SOURCE_WIDTH, \ref VX_REMAP_ATTRIBUTE_SOURCE_HEIGHT, \ref VX_REMAP_ATTRIBUTE_DESTINATION_WIDTH, \ref VX_REMAP_ATTRIBUTE_DESTINATION_HEIGHT
 * - \ref vx_lut : \ref VX_LUT_ATTRIBUTE_TYPE, \ref VX_LUT_ATTRIBUTE_COUNT
 * - \ref vx_threshold : \ref VX_THRESHOLD_ATTRIBUTE_TYPE
 * - \ref VX_META_FORMAT_ATTRIBUTE_DELTA_RECTANGLE
 * \note For vx_image, a specific attribute can be used to specify the valid region evolution. This information is not a meta data.
 *
 * \param [in] meta The reference to the \ref vx_meta_format struct to set 
 * \param [in] attribute Use the subset of data object attributes that define the meta data of this object or attributes from <tt>\ref vx_meta_format_attribute_e</tt>.
 * \param [in] ptr The input pointer of the value to set on the meta format object.
 * \param [in] size The size in bytes of the object to which \a ptr points.
 * \ingroup group_user_kernels
 * \return A <tt>\ref vx_status_e</tt> enumeration.
 * \retval VX_SUCCESS The attribute was set.
 * \retval VX_ERROR_INVALID_REFERENCE meta was not a <tt>\ref vx_meta_format</tt>.
 * \retval VX_ERROR_INVALID_PARAMETER size was not correct for the type needed.
 * \retval VX_ERROR_NOT_SUPPORTED the object attribute was not supported on the meta format object.
 * \retval VX_ERROR_INVALID_TYPE attribute type did not match known meta format type.
 */
VX_API_ENTRY vx_status VX_API_CALL vxSetMetaFormatAttribute(vx_meta_format meta, vx_enum attribute, const void *ptr, vx_size size);


#ifdef  __cplusplus
}
#endif

#endif

