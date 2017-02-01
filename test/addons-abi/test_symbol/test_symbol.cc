#include <node_jsvmapi.h>

void Test(napi_env env, napi_callback_info info) {
  napi_status status;

  int argc;
  status = napi_get_cb_args_length(env, info, &argc);
  if (status != napi_ok) return;

  if (argc < 1) {
    napi_throw_type_error(env, "Wrong number of arguments");
    return;
  }

  napi_value args[1];
  status = napi_get_cb_args(env, info, args, 1);
  if (status != napi_ok) return;

  napi_valuetype valuetype;
  status = napi_get_type_of_value(env, args[0], &valuetype);
  if (status != napi_ok) return;

  if (valuetype != napi_symbol) {
    napi_throw_type_error(env, "Wrong type of argments. Expects a symbol.");
    return;
  }

  char buffer[128];
  int buffer_size = 128;

  int remain;
  status = napi_get_string_from_value(env, args[0], buffer, buffer_size, &remain);
  if (status != napi_ok) return;

  if (remain == 0) {
    napi_value output;
    status = napi_create_string(env, buffer, &output);
    if (status != napi_ok) return;

    status = napi_set_return_value(env, info, output);
    if (status != napi_ok) return;
  }
}

void New(napi_env env, napi_callback_info info) {
  napi_status status;

  int argc;
  status = napi_get_cb_args_length(env, info, &argc);
  if (status != napi_ok) return;

  if (argc >= 1) {
    napi_value args[1];
    napi_get_cb_args(env, info, args, 1);

    napi_valuetype valuetype;
    status = napi_get_type_of_value(env, args[0], &valuetype);
    if (status != napi_ok) return;

    if (valuetype != napi_string) {
      napi_throw_type_error(env, "Wrong type of argments. Expects a string.");
      return;
    }

    char buffer[128];
    int buffer_size = 128;
    int remain;
    status = napi_get_string_from_value(env, args[0], buffer, buffer_size, &remain);
    if (status != napi_ok) return;

    if (remain  == 0) {
      napi_value symbol;
      status = napi_create_symbol(env, buffer, &symbol);
      if (status != napi_ok) return;

      status = napi_set_return_value(env, info, symbol);
      if (status != napi_ok) return;
    }
  } else {
    napi_value symbol;
    status = napi_create_symbol(env, NULL, &symbol);
    if (status != napi_ok) return;

    status = napi_set_return_value(env, info, symbol);
    if (status != napi_ok) return;
  }
}

void Init(napi_env env, napi_value exports, napi_value module) {
  napi_status status;

  napi_property_descriptor properties[] = {
    { "New", New },
  };

  for (int i = 0; i < sizeof(properties) / sizeof(*properties); i++) {
    status = napi_define_property(env, exports, &properties[i]);
    if (status != napi_ok) return;
  }
}

NODE_MODULE_ABI(addon, Init)
