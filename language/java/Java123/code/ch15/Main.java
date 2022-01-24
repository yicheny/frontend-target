package com.java123;

public class Main {
    public static void main(String[] args) throws Exception {
        BankAccount account = new BankAccount();
        account.putMoneyToBank(80);
        GetMoney money1 = new GetMoney(account,60);
        GetMoney money2 = new GetMoney(account,40);
        GetMoney money3 = new GetMoney(account,100);
        Thread moneyThread1 = new Thread(money1,"取钱线程1");
        Thread moneyThread2 = new Thread(money2,"取钱线程2");
        Thread moneyThread3 = new Thread(money3,"取钱线程3");
        moneyThread1.start();
        moneyThread2.start();
        moneyThread3.start();
    }
}
