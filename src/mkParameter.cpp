/*
 *  mkParameter.cpp
 *  MonkVG-XCode
 *
 *  Created by Micah Pearlman on 2/22/09.
 *  Copyright 2009 Monk Games. All rights reserved.
 *
 */

#include "mkCommon.h"
#include "mkContext.h"
#include "mkPath.h"

using namespace MonkVG;

VG_API_CALL VGfloat vgGetParameterf(VGHandle object, VGint paramType) {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return -1.0f;
    }

    return obj->getParameterf(paramType);
}

VG_API_CALL VGint vgGetParameteri(VGHandle object, VGint paramType) {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return -1;
    }

    return obj->getParameteri(paramType);
}

VG_API_CALL void vgSetParameterf(VGHandle object, VGint paramType,
                                 VGfloat value) {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return;
    }

    obj->setParameter(paramType, value);
}

VG_API_CALL void VG_API_ENTRY
vgSetParameterfv(VGHandle object, VGint paramType, VGint count,
                 const VGfloat *values) VG_API_EXIT {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return;
    }
    obj->setParameter(paramType, values, count);
}

VG_API_CALL void vgSetParameteri(VGHandle object, VGint paramType,
                                 VGint value) {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return;
    }

    obj->setParameter(paramType, value);
}

VG_API_CALL VGint VG_API_ENTRY
vgGetParameterVectorSize(VGHandle object, VGint paramType) VG_API_EXIT {
    throw std::runtime_error("vgGetParameterVectorSize not implemented");
    return -1;
}

VG_API_CALL void VG_API_ENTRY vgGetParameterfv(VGHandle object, VGint paramType,
                                               VGint    count,
                                               VGfloat *values) VG_API_EXIT {
    BaseObject *obj = (BaseObject *)object;
    if (obj == nullptr) {
        IContext::instance().setError(VG_BAD_HANDLE_ERROR);
        return;
    }

    obj->getParameterfv(paramType, values);
}

VG_API_CALL void VG_API_ENTRY vgGetParameteriv(VGHandle object, VGint paramType,
                                               VGint  count,
                                               VGint *values) VG_API_EXIT {
    throw std::runtime_error("vgGetParameteriv not implemented");
}
