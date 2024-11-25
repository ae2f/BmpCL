#include <ae2f/BmpCL/BmpCL.h>

static cl_program LIB = 0;

#define ae2f_BmpCLFill_ID 0
#define ae2f_BmpCLCpy_ID 1

static cl_kernel kers[] = {
    0, 0
};

ae2f_SHAREDEXPORT cl_int ae2f_BmpCLMk(
    cl_context context,
    cl_uint clDeviceNumber,
    const cl_device_id* lpDevice
) {
    cl_int _err = CL_SUCCESS;

    LIB = clCreateProgramWithSource(context, 1, &ae2f_BmpCL_Programme, 0, &_err);
    if(_err != CL_SUCCESS) return _err;
    _err = clBuildProgram(LIB, clDeviceNumber, lpDevice, 0, 0, 0);
    if(_err == CL_BUILD_SUCCESS) _err = CL_SUCCESS;
    if(_err != CL_SUCCESS) return _err;

    kers[ae2f_BmpCLFill_ID] = clCreateKernel(LIB, "ae2f_BmpCLKernFill", &_err);
    if(_err != CL_SUCCESS) return _err;

    kers[ae2f_BmpCLCpy_ID] = clCreateKernel(LIB, "ae2f_BmpCLKernCpy", &_err);
    if(_err != CL_SUCCESS) return _err;

    return _err;
}

ae2f_SHAREDEXPORT cl_int ae2f_BmpCLDel() {
    cl_int err = CL_SUCCESS;
    if(LIB) err |= clReleaseProgram(LIB); LIB = 0;
    for(size_t i = 0; i < sizeof(kers) / sizeof(cl_kernel); i++) {
        if(kers[i]) err |= clReleaseKernel(kers[i]);
        kers[i] = 0;
    }
    
    return err;
}

#include <ae2f/BmpCL/Buff.h>
#include <ae2f/Bmp/Dot.h>
#include <stdio.h>

ae2f_SHAREDEXPORT cl_int ae2f_BmpCLFill(
    ae2f_struct ae2f_cBmpCLBuff* dest, 
    cl_command_queue queue,
    ae2f_BmpDotRGBA_t colour, 
    uint32_t tcount_w,
    uint32_t tcount_h
) {
    cl_int err = 0;
    size_t workcount[2] = { tcount_w, tcount_h };
    err = clSetKernelArg(kers[ae2f_BmpCLFill_ID], 0, sizeof(cl_mem), &dest->head);
    if(err != CL_SUCCESS) return err;
    err = clSetKernelArg(kers[ae2f_BmpCLFill_ID], 1, sizeof(ae2f_BmpDotRGBA_t), &colour);
    if(err != CL_SUCCESS) return err;

    err = clEnqueueNDRangeKernel(
        queue, kers[ae2f_BmpCLFill_ID],
        2, 0, workcount, 0, 0, 0, 0
    );
    if(err != CL_SUCCESS) return err;

    err = clEnqueueReadBuffer(
        queue, dest->body, CL_TRUE, 0,
        0, dest->source->Addr, 0, 0, 0
    );
    return err;
}

ae2f_SHAREDEXPORT cl_int ae2f_BmpCLCpy(
    ae2f_struct ae2f_cBmpCLBuff* dest,
    ae2f_struct ae2f_cBmpCLBuff* src,
    const ae2f_struct ae2f_cBmpSrcCpyPrm* prm,
    cl_command_queue queue,
    uint32_t tcount_w,
    uint32_t tcount_h
) {
    cl_int err = 0;
    size_t workcount[2] = { tcount_w, tcount_h };

    err = clSetKernelArg(kers[ae2f_BmpCLCpy_ID], 0, sizeof(cl_mem), &dest->head);
    if(err != CL_SUCCESS) return err;
    err = clSetKernelArg(kers[ae2f_BmpCLCpy_ID], 1, sizeof(cl_mem), &dest->body);
    if(err != CL_SUCCESS) return err;
    err = clSetKernelArg(kers[ae2f_BmpCLCpy_ID], 2, sizeof(ae2f_struct ae2f_cBmpSrcCpyPrm), prm);
    if(err != CL_SUCCESS) return err;
}