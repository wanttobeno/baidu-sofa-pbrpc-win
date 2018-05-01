// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

#ifdef _MSC_VER
#define  usleep(x)  Sleep(x)
#endif 

// 定义回调函数
void EchoCallback(sofa::pbrpc::RpcController* cntl,
        sofa::pbrpc::test::EchoRequest* request,
        sofa::pbrpc::test::EchoResponse* response,
        bool* callbacked)
{
	// 打印一些日志
    SLOG(NOTICE, "RemoteAddress=%s", cntl->RemoteAddress().c_str());
    SLOG(NOTICE, "IsRequestSent=%s", cntl->IsRequestSent() ? "true" : "false");
    if (cntl->IsRequestSent())
    {
        SLOG(NOTICE, "LocalAddress=%s", cntl->LocalAddress().c_str());
        SLOG(NOTICE, "SentBytes=%ld", cntl->SentBytes());
    }
	 // 检查调用是否失败
    if (cntl->Failed()) {
        SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
    }
    else {
        SLOG(NOTICE, "request succeed: %s", response->message().c_str());
    }
	 // 可以在回调函数中执行一些释放资源的操作
    delete cntl;
    delete request;
    delete response;
    *callbacked = true;
}

int main()
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc server.
	// 定义RpcClient对象，管理RPC的所有资源
	// 通常来说，一个client程序只需要一个RpcClient实例
	// 可以通过RpcClientOptions指定一些配置参数，譬如线程数、流控等
    sofa::pbrpc::RpcClientOptions client_options;
	client_options.work_thread_num = 2;
	client_options.callback_thread_num = 2;

    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
	// 定义RpcChannel对象，代表一个消息通道，需传入Server端服务地址
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare parameters.
	// 定义RpcController对象，用于控制本次调用
	// 可以设置超时时间、压缩方式等；默认超时时间为3秒，默认压缩方式为无压缩
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    cntl->SetTimeout(3000);
	// 定义和填充调用方法的请求消息
    sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello,这是一段中文，请查收！\n");
    sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
    bool callbacked = false;
    google::protobuf::Closure* done = sofa::pbrpc::NewClosure(
            &EchoCallback, cntl, request, response, &callbacked);

    // Async call.
	// 定义EchoServer服务的桩对象EchoServer_Stub，使用上面定义的消息通道传输数据
    sofa::pbrpc::test::EchoServer_Stub stub(&rpc_channel);
	// 发起调用，最后一个参数为NULL表示为同步调用
    stub.Echo(cntl, request, response, done);

    // Wait call done.
    while (!callbacked) {
        usleep(100000);
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
