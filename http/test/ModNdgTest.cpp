// Copyright (c) 2015
// Author: Chrono Law
//#include "NdgTestInit.hpp"

//auto ndg_test_module = NdgTestInit::module();

#include <iostream>

#include "NgxAll.hpp"

// 配置的数据结构
struct NdgTestConf final						 // 禁止被继承
{
	// ngx_flag_t 类型可以把配置文件里的 on|off 信息转换为 1|0 保存 
    ngx_flag_t enabled = NgxUnsetValue::get();	 // 标志变量，构造时初始化
};

static void *create(ngx_conf_t* cf);
static char *merge(ngx_conf_t *cf, void *parent, void *child);

// 配置指令解析
static ngx_command_t ndg_test_cmds[] =			 // 配置指令数组
{
    {
        ngx_string("ndg_test"),					 // 指令的名字
        NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,		 // 指令的作用域和类型
        ngx_conf_set_flag_slot,					 // 解析函数指针
        NGX_HTTP_LOC_CONF_OFFSET,				 // 数据的存储位置
        offsetof(NdgTestConf, enabled),			 // 数据的具体存储变量
        nullptr									 // 暂无须关心
    },

    ngx_null_command							 // 空对象，结束数组
};

static ngx_int_t init(ngx_conf_t* cf);
static ngx_int_t handler(ngx_http_request_t *r);

static ngx_http_module_t ndg_test_ctx =
{
    nullptr,
    init,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    create,
    nullptr,//merge
};

ngx_module_t ndg_test_module =                   // 模块定义,注意不是static
{
    NGX_MODULE_V1,                               // 标准的填充宏 
    &ndg_test_ctx,                               // 配置功能函数 
    ndg_test_cmds,                               // 配置指令数组
    NGX_HTTP_MODULE,                             // http模块必须的tag
    nullptr,                                     // init master
    nullptr,                                     // init module
    nullptr,                                     // init process
    nullptr,                                     // init thread
    nullptr,                                     // exit thread
    nullptr,                                     // exit process
    nullptr,                                     // exit master
    NGX_MODULE_V1_PADDING                        // 标准的填充宏 
};

/* *
 * Nginx要求模块自己分配配置数据结构的内存
 * */
// 创建配置数据
static void* create(ngx_conf_t* cf)
{												
    return NgxPool(cf).alloc<NdgTestConf>();	 // 由内存池分配内存，构造对象	
}

static char *merge(ngx_conf_t *cf, void *parent, void *child)
{
    boost::ignore_unused(cf);

    auto prev = reinterpret_cast<NdgTestConf*>(parent);
    auto conf = reinterpret_cast<NdgTestConf*>(child);

    NgxValue::merge(conf->enabled, prev->enabled, 1);

    return NGX_CONF_OK;
}

// 注册处理函数
static ngx_int_t init(ngx_conf_t* cf)
{
    auto cmcf = reinterpret_cast<ngx_http_core_main_conf_t*>(
        ngx_http_conf_get_module_main_conf(             // 获取函数
            cf, ngx_http_core_module));                 // 使用的函数

    NgxArray<ngx_http_handler_pt> arr(                  // Nginx数组
        cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers); // 该阶段的handler

    arr.push(handler);                                  // 加入自己的handler

    return NGX_OK;                                      // 执行成功
}

// 处理函数
static ngx_int_t handler(ngx_http_request_t *r)
{
    auto cf = reinterpret_cast<NdgTestConf*>(       // 获取配置数据
        ngx_http_get_module_loc_conf(r, ndg_test_module));

    NgxLogError(r).print("hello c++");              // 记录运行日志

    if (cf->enabled)                                // 检查配置参数
    {
        std::cout << "hello nginx" << std::endl;    // 输出字符串
    }
    else
    {
        std::cout << "hello disabled" << std::endl; // 输出字符串
    }


    return NGX_DECLINED;                            // 执行成功但未处理
}

