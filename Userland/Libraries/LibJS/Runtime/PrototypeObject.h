/*
 * Copyright (c) 2021, Tim Flynn <trflynn89@pm.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/StringView.h>
#include <AK/TypeCasts.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/Object.h>

namespace JS {

#define JS_PROTOTYPE_OBJECT(prototype_class, object_class, display_name_) \
    using Prototype = PrototypeObject<prototype_class, object_class>;     \
    JS_OBJECT(prototype_class, Prototype)                                 \
    static constexpr StringView display_name() { return #display_name_##sv; }

template<typename PrototypeType, typename ObjectType>
class PrototypeObject : public Object {
    JS_OBJECT(PrototypeObject, Object);

public:
    virtual ~PrototypeObject() override = default;

    static Object* this_object(GlobalObject& global_object)
    {
        auto& vm = global_object.vm();

        auto this_value = vm.this_value(global_object);

        if (!this_value.is_object()) {
            vm.throw_exception<TypeError>(global_object, ErrorType::NotAnObject, this_value);
            return nullptr;
        }

        return &this_value.as_object();
    }

    // Use typed_this_object() when the spec coerces |this| value to an object.
    static ObjectType* typed_this_object(GlobalObject& global_object)
    {
        auto& vm = global_object.vm();

        auto* this_object = vm.this_value(global_object).to_object(global_object);
        if (!this_object)
            return nullptr;

        if (!is<ObjectType>(this_object)) {
            vm.throw_exception<TypeError>(global_object, ErrorType::NotAnObjectOfType, PrototypeType::display_name());
            return nullptr;
        }

        return static_cast<ObjectType*>(this_object);
    }

    // Use typed_this_value() when the spec does not coerce |this| value to an object.
    static ObjectType* typed_this_value(GlobalObject& global_object)
    {
        auto& vm = global_object.vm();

        auto this_value = vm.this_value(global_object);

        if (!this_value.is_object() || !is<ObjectType>(this_value.as_object())) {
            vm.throw_exception<TypeError>(global_object, ErrorType::NotAnObjectOfType, PrototypeType::display_name());
            return nullptr;
        }

        return static_cast<ObjectType*>(&this_value.as_object());
    }

protected:
    explicit PrototypeObject(Object& prototype)
        : Object(prototype)
    {
    }
};

}
