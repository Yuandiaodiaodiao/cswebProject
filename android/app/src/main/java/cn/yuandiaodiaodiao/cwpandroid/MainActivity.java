package cn.yuandiaodiaodiao.cwpandroid;

import android.annotation.SuppressLint;
import android.os.Debug;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.TabLayout;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class MainActivity extends AppCompatActivity {
    String username = "";
    String ip="";
    TabLayout tab;
    TextView tv;
    HashSet<String> onlineName = new HashSet<String>();
    HashMap<String, String> msgMap = new HashMap<String, String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tab = (TabLayout) findViewById(R.id.tabLayout);
        tv = (TextView) findViewById(R.id.textView);
        Button loginButton = (Button) findViewById(R.id.button3);
        Button sendButton=(Button)findViewById(R.id.button2);

        sendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText et=(EditText)findViewById(R.id.editText2);
                String sendMsg=et.getText().toString();
                et.setText("");
                String to=tab.getTabAt(tab.getSelectedTabPosition()).getText().toString();
                Thread t1 = new Thread(new Runn("send$" + username+"$"+to+"$"+sendMsg, 4));
                t1.start();
                if (!msgMap.containsKey(to)) msgMap.put(to, "");
                msgMap.put(to, msgMap.get(to) + username + " : " + sendMsg + "\n");
                tv.setText(msgMap.get(to));
            }
        });
        tab.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                String s = tab.getText().toString();
                tv.setText(msgMap.get(s));
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {
            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {
            }
        });

        loginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText T1 = (EditText) findViewById(R.id.editText3);
                username = T1.getText().toString();
                if (username.length() <= 2) {
                    Toast.makeText(getApplicationContext(), "用户名太短 登录失败", Toast.LENGTH_SHORT).show();
                    return;
                }
                EditText ipt=(EditText)findViewById(R.id.editText4);
                ip=ipt.getText().toString();
                Thread t1 = new Thread(new Runn("login$" + username, 1));
                t1.start();
                try {
                    t1.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                Thread t2 = new Thread(new Runn("getonline$" + username, 2));
                t2.start();
                try {
                    t2.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                //登陆成功
                //轮询在线list和未读消息
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        while (true) {
                            Thread t1 = new Thread(new Runn("getonline$" + username, 2));
                            Thread t2 = new Thread(new Runn("getmsg$" + username, 3));
                            try {
                                t1.start();
                                Thread.sleep(500);
                                t2.start();
                                t1.join();
                                t2.join();
                                Thread.sleep(500);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();
            }
        });


    }


    @SuppressLint("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1://login
                    break;
                case 2://getonline
                    String[] sarray = msg.obj.toString().split("\\$");
                    for (String s : sarray) {
                        if (!onlineName.contains(s) && s.length() > 2) {
                            tab.addTab(tab.newTab().setText(s));
                            onlineName.add(s);
                            if(onlineName.size()==1)tab.getTabAt(0).select();
                        }
                    }
                    break;
                case 3://getmessage

                    String[] sarray2 = msg.obj.toString().split("\n");
                    for (String s : sarray2) {
                        if (s.length() <= 2) continue;
//                        Toast.makeText(getApplicationContext(),"切分"+s,Toast.LENGTH_SHORT).show();
                        String[] sarray3 = s.split("\\$");
                        String to="";
                        String tomsg="";
                        for (String s2 : sarray3) {
                            if(to.length()>2){
                                tomsg=s2;
                                break;
                            }
                            if(s2.length()<=2)continue;
                            to=s2;
                        }
                        if(to.length()<=2||tomsg.length()==0)continue;
                        if (!msgMap.containsKey(to)) msgMap.put(to, "");
                        msgMap.put(to, msgMap.get(to) + to + " : " + tomsg + "\n");
                        int x=Math.max(tab.getSelectedTabPosition(),0);
//                        tv.setText("?"+tab.getTabAt(x).getText().toString()+"?   ?"+to+"?");
                        if (tab.getTabAt(x).getText().toString().equals(to)) {
//                            tv.setText("?"+tab.getTabAt(x).getText().toString()+"?   ?"+to+"?");
                            tv.setText(msgMap.get(to));
                        }
                    }
                    break;
                case 4:
                    Toast.makeText(getApplicationContext(),msg.obj.toString(),Toast.LENGTH_SHORT).show();
                    break;
                default:
                    break;
            }
        }


    };


    public class Runn implements Runnable {
        private String op;
        private int what;

        public Runn(String s, int whats) {
            this.op = s;
            this.what = whats;
        }

        @Override
        public void run() {

            Message msg = new Message();
            Socket socket;
            try {// 创建一个Socket对象，并指定服务端的IP及端口号
                //101.76.243.181
                //101.76.251.152
                socket = new Socket(ip, 12347);
                String socketData = this.op;
                BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
                        socket.getOutputStream(),"GBK"));
                writer.write(socketData);
                writer.flush();
                BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), "GBK"));
                String s = "";
                String buffs;
                while ((buffs = reader.readLine()) != null) {
                    s += buffs + "\n";
                }
                msg.obj = s;

                writer.close();
                reader.close();
                socket.close();
            } catch (UnknownHostException e) {
                msg.obj = "UnknownHostException";
                return;
            } catch (IOException e) {
                msg.obj = "IOException";
                return;
            }
            msg.what = this.what;
            handler.sendMessage(msg);
        }
    }
}
