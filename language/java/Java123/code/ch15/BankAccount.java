package com.java123;

public class BankAccount {
    private int money = 0;

    public synchronized void getMoneyOutOfBank(int cash) throws Exception {
        if(cash <= 0) throw new Exception("取钱数目必须大于0！");
        if(money < cash) throw new Exception("存款不足！");
        System.out.println("正在取钱中，请稍等……");
        try{
            Thread.sleep(1000);
        }catch (InterruptedException e){
            System.out.println("程序运行出错，错误信息：" + e.getMessage());
            return ;
        }
        money = money - cash;
        System.out.println("取钱成功，请拿好现金：" + cash + "元。现在账户余额为：" + money + "元");
    }

    public void putMoneyToBank(int cash) throws Exception {
        if(cash <= 0) throw new Exception("存钱数额必须大于0");
        System.out.println("正在存钱中，请稍等……");
        try {
            Thread.sleep(1000);
        }catch (InterruptedException e){
            System.out.println("程序运行出错，错误信息：" + e.getMessage());
            return ;
        }
        money += cash;
        System.out.println("存钱成功，金额为：" + cash + "元。现在账户余额为：" + money + "元");
    }

    public void peekMoney(){
        System.out.println("现在账户余额为：" + money + "元。");
    }
}
