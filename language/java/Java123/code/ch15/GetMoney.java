package com.java123;

public class GetMoney implements Runnable{
    private final BankAccount account;
    private final int cash;

    public GetMoney(BankAccount account, int cash){
        this.account = account;
        this.cash = cash;
    }

    public void run(){
        String name = Thread.currentThread().getName();
        System.out.println("线程：" + name + "开始取钱！");
        try {
            account.getMoneyOutOfBank(cash);
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
        System.out.println("线程：" + name + "取钱完毕！");
    }
}
