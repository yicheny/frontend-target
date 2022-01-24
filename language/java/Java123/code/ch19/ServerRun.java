package com.java123;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

public class ServerRun {
    public static void main(String[] args){
        try{
            ServerSocket serverSocket = new ServerSocket(8090);
            System.out.println("开始监听8090端口……");
            Socket socket = serverSocket.accept();
            System.out.println("有客户成功连接，开始进行通信……");
            OutputStream outputStream = socket.getOutputStream();
            PrintWriter printWriter = new PrintWriter(outputStream);
            System.out.println("正在向客户端发送消息……");
            printWriter.write("你好，这里是服务端！");
            printWriter.flush();
            System.out.println("向客户端发送消息完成！");
            InputStream inputStream = socket.getInputStream();
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String response = bufferedReader.readLine();
            System.out.println("======下面是客户端发送的信息======");
            System.out.println(response);
            System.out.println("==============================");
            bufferedReader.close();
            printWriter.close();
            socket.close();
            serverSocket.close();
        }catch (UnknownHostException e){
            System.out.println("无法找到相应的机器" + e.getMessage());
        } catch (IOException e) {
            System.out.println("数据传输异常：" + e.getMessage());
        }
    }
}
