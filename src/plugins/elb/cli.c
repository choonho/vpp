#include <elb/elb.h>
#include <elb/util.h>

static clib_error_t *
elb_create_command_fn (vlib_main_t * vm,
                    unformat_input_t * input, vlib_cli_command_t * cmd)
{
  unformat_input_t _line_input, *line_input = &_line_input;
  u8 *elb_name = NULL;
  clib_error_t *error = 0;
  ip46_address_t host;
  u16 port;
  u8 mode = 0;
  u8 del = 0;
  int ret;

  if (!unformat_user (input, unformat_line_input, line_input))
    return 0;

  if (!unformat(line_input, "frontend %s", &elb_name))
    goto done;

  while (unformat_check_input (line_input) != UNFORMAT_END_OF_INPUT)
  {
    if (unformat(line_input, "bind %U", unformat_ip46_bind, &host, &port, IP46_TYPE_ANY));
    else if (unformat(line_input, "del"))
      del = 1;
    else {
      error = clib_error_return(0, "unknown input `%U'",
      format_unformat_error, line_input);
      goto done;
    }
  }

  u32 index;
  if (!del) {
    // Create ELB
    if ((ret = elb_frontend_add(elb_name, &host, port, mode, &index))) {
      error = clib_error_return (0, "elb_frontend_add error %d", ret);
      goto done;
    } else {
      vlib_cli_output(vm, "elb_frontend_add ok %d", index);
    }
  } else {
    // Delete ELB
    goto done;
  }

done:
    vec_free (elb_name);
    unformat_free (line_input);

    return error;
}

VLIB_CLI_COMMAND (elb_create_command, static) =
{
    .path = "elb create",
    .short_help = "elb create frontend <name> [bind <ip:port>] [mode [tcp|http|https]] [del]",
    .function = elb_create_command_fn,
};

static clib_error_t *
elb_show_command_fn (vlib_main_t *vm,
                unformat_input_t * input, vlib_cli_command_t * cmd)
{
  vlib_cli_output(vm, "%U", format_elb_main);
  return NULL;
}

VLIB_CLI_COMMAND (elb_show_command, static) =
{
  .path = "show elb",
  .short_help = "show elb",
  .function = elb_show_command_fn,
};

