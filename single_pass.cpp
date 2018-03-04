#include <iostream>
#include <vector>
#include<algorithm>
#include<math.h>

using namespace std;

class document{
    
    public:
    vector<string> terms;
    int degree = 0;
    
    
    document(){
        string s;
        int i,j;
        //manual add
        cout<<"no of terms\n";
        cin>>i;
        for(j=0;j<i;j++){
            cin>>s;
            terms.push_back(s);
        }//end for
        do_sort();
     
    }
    
    
    float size(){
       return sqrt(terms.size()); 
    }
    
    void print(){
        for (auto i = terms.begin(); i != terms.end(); ++i){
            cout << *i << ' ';
        }
        cout<<"\n";
    }
    
    void do_sort(){
        sort(terms.begin(), terms.end());
    }
    
    
};



class cluster{
    
    vector<document*> docs;
    document *cr;
    
    void new_cr(document *d){
        cr = d;
    }
    
    public:
    
    void add_doc(document *d){
        docs.push_back(d);
        new_cr(d);
    }
    
    float dice_coeff(document *d){
        //intersection between cr and d
        int intersection = intersect(cr->terms,d->terms);
        //cout<<intersection<<"\n";
        //cr->print();
        //d->print();
        float dice = ((float)intersection)/(cr->size() * d->size());
        cout<<intersection<<" "<<cr->size()<<" "<<d->size()<<" dice \n";
        return dice;
    }
    
    int intersect(vector<string> &v1, vector<string> &v2){

        //vector<string> v11 = new vector(v1);
        //vector<string> v22 = new vector(v2);
        vector<string> v3;
        
        //sort(v1.begin(), v1.end());
        //sort(v2.begin(), v2.end());
        
        set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));
        
        return v3.size();
        
    }
    
    
};


vector<cluster> cl;

void determine_cluster(document *d){
    int new_cluster=1; //flag
    float dc;
    cluster *best_cluster;
    int max = 0; //for best cluster
    float threshold = 0.6;
    if(cl.size() == 0){
        cluster c;
        c.add_doc(d);
        cl.push_back(c);
    }else{
        for (vector<cluster>::iterator it = cl.begin() ; it != cl.end(); ++it){
            dc = it->dice_coeff(d);
            cout<<dc<<"\n";
            if(dc > max){
                max = dc;
                best_cluster = &*it;
                //it->add_doc(d);
                //new_cluster = 0;
                //break;
            }
        }
        
        if(max > threshold){
            best_cluster->add_doc(d);
            new_cluster = 0;
        }
        
        if(new_cluster){
            cluster c;
            c.add_doc(d);
            cl.push_back(c);
            cout<<"new cluster made"<<"\n";
        }
        
        
    }
    
    
    
}


int main() {
    int total_docs,i;
    cout<<"No. of documents\n";
    cin>>total_docs;
    i = total_docs;
    while(i--){
        determine_cluster(new document());
    }
    
    cout<<"No. of clusters formed is/are "<<cl.size()<<"\n";
    
	return 0;
}
