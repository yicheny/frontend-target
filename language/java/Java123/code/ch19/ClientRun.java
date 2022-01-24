package com.java123;

import java.io.*;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class ClientRun {
    public static void main(String[] args){
        byte[] addr = new byte[] {127,0,0,1};
        try{
            System.out.println("======客户端请求数据……======");
            InetAddress local = InetAddress.getByAddress(addr);
            Socket socket = new Socket(local,8090);
            InputStream inputStream = socket.getInputStream();

//            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
//            String response = bufferedReader.readLine();

            byte[] data = new byte[1024];
            String response = new String(data,0,inputStream.read(data));

            System.out.println("======下面是服务端发送的消息======");
            System.out.println(response);
            System.out.println("==============================");
            System.out.println("正在向服务端发送消息……");
            OutputStream outputStream = socket.getOutputStream();
            PrintWriter printWriter = new PrintWriter(outputStream);
            printWriter.write("服务器你好，这里是客户端!");
            printWriter.flush();
            printWriter.close();
//            bufferedReader.close();
            socket.close();
            System.out.println("向服务端发送数据结束！");
        }catch (UnknownHostException e){
            System.out.println("无法找到相应的机器" + e.getMessage());
        } catch (IOException e) {
            System.out.println("数据传输异常：" + e.getMessage());
        }
    }
}