/*
KMeans

*/

import java.util.Scanner;
import java.util.Random;
import java.util.ArrayList;
import java.util.HashMap;

public class kmeans {

    final int max_v = 100, var = 0;
    final int clusters_total;
    int epoch = 0;
    final int MAX_EPOCHS = 1000;
    ArrayList<cluster> clusters = new ArrayList<>();
    ArrayList<data> datas = new ArrayList<>();
    HashMap<Integer, Integer> hm = new HashMap<>();
    HashMap<Integer, ArrayList<Integer>> ans = new HashMap<>();

    class data {

        final int data_hash;
        cluster c;
        final float x, y;

        data(float x, float y, int data_hash) {
            this.x = x;
            this.y = y;
            this.data_hash = data_hash;
        }

    }

    class cluster {

        final int cluster_no;
        float x, y;
        float sum_x = 0, sum_y;
        int num = 0;

        cluster(float x, float y, int cluster_no) {
            this.x = x;
            this.y = y;
            this.cluster_no = cluster_no;
        }

        float distance(float x, float y) {
            return (float) (Math.pow(this.x - x, 2) + Math.pow(this.y - y, 2));
        }

        void sum_add(float sumx, float sumy) {
            this.sum_x += sumx;
            this.sum_y += sumy;
            num++;
        }

        void new_centroid() {
//System.out.println(
            this.x = this.sum_x / num;
            this.y = this.sum_y / num;
            sum_x = 0;
            sum_y = 0;
            num = 0;
        }

    }

    void find_cluster(data d) {
        float min = Float.MAX_VALUE;
        cluster c_final = null;
        for (cluster c : clusters) {
            float dis = c.distance(d.x, d.y);
            if (dis < min) {
                min = dis;
                c_final = c;
            }//end if
        }//end for
        d.c = c_final;

    }

    void assign() {

        for (data d : datas) {
            find_cluster(d);
        }

    }//end assign

    void calc_new_centroid() {

        for (data d : datas) {
            d.c.sum_add(d.x, d.y);
        }

        for (cluster c : clusters) {
            c.new_centroid();
        }

    }//end func

    void print() {

        for (cluster c : clusters) {
            System.out.println("Cluster No. " + c.cluster_no + " || Centroid -> " + c.x + " " + c.y);
        }

        for (data d : datas) {
            System.out.println(d.x + " " + d.y + " || cluster No. " + d.c.cluster_no);
        }

        System.out.println("----------------------------------------------");
    }

    boolean stop_check() {
        boolean b = true;
        for (data d : datas) {
            int c_no = hm.get(d.data_hash);
            if (c_no != d.c.cluster_no) {
                b = false;
                break;
            }//end if
        }//end for

        return b;

    }

    void dump_data() {
        hm.clear();
        for (data d : datas) {
            hm.put(d.data_hash, d.c.cluster_no);
        }

    }

    void populate_ans() {
        for (data d : datas) {
            if (ans.get(d.c.cluster_no) == null) {
                ans.put(d.c.cluster_no, new ArrayList<>());
            }

            ans.get(d.c.cluster_no).add(d.data_hash);
        }

        for (cluster c : clusters) {
            System.out.println("Cluster no " + c.cluster_no);
            ArrayList<Integer> ar = ans.get(c.cluster_no);
            for (int i : ar) {
                System.out.print("#" + i + " ");
            }
            System.out.println();
        }

    }
    
    
    void random_input(Scanner s){
        Random r = new Random();
        
        for (int i = 0; i < clusters_total; i++) {
            clusters.add(new cluster(r.nextInt(max_v) - var, r.nextInt(max_v) - var, i));
        }

        System.out.println("Enter number of input data");
        int inputs = s.nextInt();
        for (int i = 0; i < inputs; i++) {
            datas.add(new data(r.nextInt(max_v) - var, r.nextInt(max_v) - var, i));
        }
    }

    void run_epochs(){
        boolean first = true;
        for (int i = 0; i < MAX_EPOCHS; i++) {
            epoch++;
            assign();
            calc_new_centroid();
            //print();
            if (!first) {
                if (stop_check()) {
                    break;
                }
            }//end if
            first = false;
            dump_data();
        }//end for
        System.out.println("Number of epochs is/are " + epoch);
    }
    
    kmeans() {
        
        Scanner s = new Scanner(System.in);
        System.out.println("Enter number of clusters");
        clusters_total = s.nextInt();       
        random_input(s);
        run_epochs();
        populate_ans();

    }//end fun

    public static void main(String args[]) {
        new kmeans();
    }

}
