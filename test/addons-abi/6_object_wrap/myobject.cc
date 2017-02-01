#include "myobject.h"

napi_persistent MyObject::constructor;

MyObject::MyObject(double value) : value_(value) {
}

MyObject::~MyObject() {
}

void MyObject::Destructor(void* nativeObject) {
  reinterpret_cast<MyObject*>(nativeObject)->~MyObject();
}

void MyObject::Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor properties[] = {
    { "value", nullptr, GetValue, SetValue },
    { "plusOne", PlusOne },
    { "multiply", Multiply },
  };

  napi_value cons;
  status = napi_create_constructor(env, "MyObject", New, nullptr, 3, properties, &cons);
  if (status != napi_ok) return;

  status = napi_create_persistent(env, cons, &constructor);
  if (status != napi_ok) return;

  napi_propertyname name;
  status = napi_property_name(env, "MyObject", &name);
  if (status != napi_ok) return;

  status = napi_set_property(env, exports, name, cons);
  if (status != napi_ok) return;
}

void MyObject::New(napi_env env, napi_callback_info info) {
  napi_status status;

  bool is_constructor;
  status = napi_is_construct_call(env, info, &is_constructor);
  if (status != napi_ok) return;

  if (is_constructor) {
    // Invoked as constructor: `new MyObject(...)`
    napi_value args[1];
    status = napi_get_cb_args(env, info, args, 1);
    if (status != napi_ok) return;

    double value = 0;

    napi_valuetype valuetype;
    status = napi_get_type_of_value(env, args[0], &valuetype);
    if (status != napi_ok) return;

    if (valuetype != napi_undefined) {
      status = napi_get_number_from_value(env, args[0], &value);
      if (status != napi_ok) return;
    }

    MyObject* obj = new MyObject(value);

    napi_value jsthis;
    status = napi_get_cb_this(env, info, &jsthis);
    if (status != napi_ok) return;

    status = napi_wrap(env, jsthis, reinterpret_cast<void*>(obj),
                       MyObject::Destructor, nullptr);
    if (status != napi_ok) return;

    status = napi_set_return_value(env, info, jsthis);
    if (status != napi_ok) return;
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    napi_value args[1];
    status = napi_get_cb_args(env, info, args, 1);
    if (status != napi_ok) return;

    const int argc = 1;
    napi_value argv[argc] = { args[0] };

    napi_value cons;
    status = napi_get_persistent_value(env, constructor, &cons);
    if (status != napi_ok) return;

    napi_value instance;
    status = napi_new_instance(env, cons, argc, argv, &instance);
    if (status != napi_ok) return;

    status = napi_set_return_value(env, info, instance);
    if (status != napi_ok) return;
  }
}

void MyObject::GetValue(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value jsthis;
  status = napi_get_cb_this(env, info, &jsthis);
  if (status != napi_ok) return;

  MyObject* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  if (status != napi_ok) return;

  napi_value num;
  status = napi_create_number(env, obj->value_, &num);
  if (status != napi_ok) return;

  status = napi_set_return_value(env, info, num);
  if (status != napi_ok) return;
}

void MyObject::SetValue(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value value;
  status = napi_get_cb_args(env, info, &value, 1);
  if (status != napi_ok) return;

  napi_value jsthis;
  status = napi_get_cb_this(env, info, &jsthis);
  if (status != napi_ok) return;

  MyObject* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  if (status != napi_ok) return;

  status = napi_get_number_from_value(env, value, &obj->value_);
  if (status != napi_ok) return;
}

void MyObject::PlusOne(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value jsthis;
  status = napi_get_cb_this(env, info, &jsthis);
  if (status != napi_ok) return;

  MyObject* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  if (status != napi_ok) return;

  obj->value_ += 1;

  napi_value num;
  status = napi_create_number(env, obj->value_, &num);
  if (status != napi_ok) return;

  status = napi_set_return_value(env, info, num);
  if (status != napi_ok) return;
}

void MyObject::Multiply(napi_env env, napi_callback_info info) {
  napi_status status;

  napi_value args[1];
  status = napi_get_cb_args(env, info, args, 1);
  if (status != napi_ok) return;

  napi_valuetype valuetype;
  status = napi_get_type_of_value(env, args[0], &valuetype);
  if (status != napi_ok) return;

  double multiple = 1;
  if (valuetype != napi_undefined) {
    status = napi_get_number_from_value(env, args[0], &multiple);
    if (status != napi_ok) return;
  }

  napi_value jsthis;
  status = napi_get_cb_this(env, info, &jsthis);
  if (status != napi_ok) return;

  MyObject* obj;
  status = napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj));
  if (status != napi_ok) return;

  napi_value cons;
  status = napi_get_persistent_value(env, constructor, &cons);
  if (status != napi_ok) return;

  const int argc = 1;
  napi_value argv[argc];
  status = napi_create_number(env, obj->value_ * multiple, argv);
  if (status != napi_ok) return;

  napi_value instance;
  status = napi_new_instance(env, cons, argc, argv, &instance);
  if (status != napi_ok) return;

  status = napi_set_return_value(env, info, instance);
  if (status != napi_ok) return;
}
