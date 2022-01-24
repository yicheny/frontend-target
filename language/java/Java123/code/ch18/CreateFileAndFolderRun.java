package com.java123;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class CreateFileAndFolderRun {
    public static void main(String[] args) {
        String commonPath = "F:\\my-2021\\IdeaProjects\\src\\com\\java123\\temp";
        SimpleFile simpleFile = new SimpleFile(commonPath);
        simpleFile.printFileList("testCreateFolder");

//        simpleFile.createFolder("testCreateFolder");
//        simpleFile.createFile("testCreateFile.txt");
//        simpleFile.delete("testCreateFolder");
//        simpleFile.delete("testCreateFile.txt");
    }
}

class SimpleFile{
    private final String commonPath;

    SimpleFile(String commonPath){
        this.commonPath = commonPath;
    }

    void createFolder(String folderName){
        File folder = new File(getPath(folderName));
        if(folder.exists() && folder.isDirectory()){
            System.out.println(folderName + "文件已存在！");
        }else{
            if(folder.mkdirs()){
                System.out.println(folderName + "创建成功！");
            }else{
                System.out.println(folderName + "创建失败！");
            }
        }
    }

    void createFile(String fileName) throws IOException {
        File file = new File(getPath(fileName));

        if(file.exists() && file.isFile()){
            System.out.println(fileName + "文件已存在！");
        }else{
            if(file.createNewFile()){
                System.out.println(fileName + "创建成功！");
            }else{
                System.out.println(fileName + "创建失败！");
            }
        }
    }

    void delete(String name){
        File file = new File(getPath(name));
        if(!file.exists()) {
            System.out.println(name + "文件不存在");
            return ;
        }
        if(file.delete()){
            System.out.println(name + "删除成功");
        }else{
            System.out.println(name + "删除失败");
        }
    }

    void printFileList(String name){
        File dir = new File(getPath(name));
        if(!dir.exists()) {
            System.out.println("文件夹不存在");
            return ;
        }
        if(dir.isDirectory()) {
            File[] allFiles = dir.listFiles();
            ArrayList<File> files = new ArrayList<>();
            ArrayList<File> folders = new ArrayList<>();
            assert allFiles != null;
            for (File currentFile : allFiles) {
                if (currentFile.isFile()) {
                    files.add(currentFile);
                } else {
                    folders.add(currentFile);
                }
            }
            if(folders.size() != 0) {
                System.out.println("文件夹" + name + "包含如下文件夹：");
                printListInfo(folders);
            }else{
                System.out.println("文件夹" + name + "不包含文件夹");
            }

            if(files.size() != 0) {
                System.out.println("文件夹" + name + "包含如下文件：");
                printListInfo(files);
            }else{
                System.out.println("文件夹" + name + "不包含文件");
            }
        }else{
            System.out.println("这不是一个文件夹");
        }
    }

    private void printListInfo(ArrayList<File> list){
        for (File file : list){
            System.out.println(file.getName());
        }
    }

    private String getPath(String name){
        return commonPath + File.separator + name;
    }
}