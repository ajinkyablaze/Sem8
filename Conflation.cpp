#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string data; /*Global*/

void StopWordRemoval(string inputfile)
{
	data.clear();
	string temp;
	string stopwords[300]; 
	int i,num;
	fstream ipfile,opfile,StopFile;
	char c;

	StopFile.open("StopWords.txt",ios::in);
	
	for(i=0;StopFile>>stopwords[i];i++)
	{
		//cout<<stopwords[i];//Read all Stop Words and save in array of string stopwords
	}
	num=i;
	StopFile.close();
	
	ipfile.open("conflation_input/" + inputfile,ios::in);	
	opfile.open("StopWordsRemoved.txt",ios::out);
	//cout<<"yolo"<<num<<" "<<inputfile<<endl;
	/*while(ipfile >> temp){
		//cout<<"here"<<endl;
		for(i=0;i<num;i++){
			//cout<<temp<<" "<<stopwords[i]<<endl;
			if(!data.compare(stopwords[i]))
				break;
		}
		if(i==num)
			opfile<<data<<" ";
	}*/	
	
	while(ipfile.get(c))
	{
		if(!isspace(c)) // Only alpha, remove punctuation
		{
			data.push_back(toupper(c)); //Add char to data
		}
		else
		{
			for(i=0;i<num;i++)
			{
				if(data==stopwords[i])
					break;
			}
			if(i==num)
				opfile<<data<<" ";

			data.clear(); //Clear string data
		}	
	}
	
	ipfile.close();
	opfile.close();
}

/* cons(i) is true if data[i] is a consonant. 
   Y is a vowel if it is preceded by a consonant*/
bool cons(int i)
{  switch (data[i])
   {  case 'A': case 'E': case 'I': case 'O': case 'U': return false;
      case 'Y': return (i==0) ? true : !cons(i-1);
      default: return true;
   }
}

/* m() measures the number of consonant sequences in data from 0 to len (till end of stem).
   if c is a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
   presence,

      <c><v>       gives 0
      <c>vc<v>     gives 1
      <c>vcvc<v>   gives 2
      <c>vcvcvc<v> gives 3
      ....
*/
int m(int len)
{  
	int i=0,n=0;
	
	while(true)
	{  
		if (i >= len) 
			return n;
		if (! cons(i)) 
			break;
		i++;
	}
	i++;
	
	while(true)
	{  
		while(true)
		{
			if (i >= len) 
				return n;
			if (cons(i)) 
				break;
			i++;
		}
		i++;
		n++;
		while(true)
		{  
			if (i >= len)
				return n;
			if (! cons(i)) 
				break;
			i++;
		}
		i++;
	}
	
	return n;
}

/* vowelInStem() is true if data contains a vowel */
bool vowelInStem(int len)
{  
	int i;
	for (i = 0; i < len; i++) 
		if (!cons(i)) 
			return true;
   	return false;
}

//Returns 1 if original ends in ending
int endsWith(string ending)
{
	int orilen = data.size();
	int endlen = ending.size();
	
	if(orilen>=endlen)
		return !(data.compare(orilen-endlen,endlen,ending));
	else 
		return 0;
}

/* doublec() is true if data ends with a double consonant. */
bool doublec()
{
	int len = data.size();
   	if (data[len-1] != data[len-2]) 
   		return false;
   	return cons(len-1);
}

/* cvc(i) is true if i-2,i-1,i has the form consonant - vowel - consonant
   and also if the second c is not w,x or y.
*/
bool cvc(int i)
{  
	if (!cons(i) || cons(i-1) || !cons(i-2)) 
		return false;
	
	char ch = data[i];
	if (ch == 'w' || ch == 'x' || ch == 'y') 
		return false;
	
	return true;
}

/*
	SSES -> SS
	IES  -> I
	SS   -> SS
	S    ->                            
*/
void Suffix_Step1a()
{
	int len = data.size();

	if(endsWith("SSES"))
	{	
		data.replace(len-4,4,"SS");
	}
	else if(endsWith("IES"))
	{	
		data.replace(len-3,3,"I");
	}
	else if(endsWith("SS"))
	{
		//Stays as is
	}
	else if(endsWith("S"))
	{	
		data.replace(len-1,1,"");
	}
	
}

/*
	(m>0) EED -> EE
	(*v*) ED  ->
	(*v*) ING ->
	
	If the second or third of the rules in Step 1b is successful, the following is done:
	AT -> ATE
	BL -> BLE
	IZ -> IZE
	(*d and not (*L or *S or *Z)) -> single letter
	(m=1 and *o) -> E
*/
void Suffix_Step1b()
{
	char ch;
	int len = data.size();
	bool flag = false; // To indicate if 2nd or 3rd rule successfull
	
	if(endsWith("EED"))
	{
		if(m(len-3)>0)
			data.replace(len-3,3,"EE");
	}
	else if(endsWith("ED")&& vowelInStem(len-2))
	{
		data.replace(len-2,2,"");
		flag = true;
	}
	else if(endsWith("ING") && vowelInStem(len-3))
	{
		data.replace(len-3,3,"");
		flag = true;
	}
	
	len = data.size(); // len may change after above trimming
	if(flag)
	{
		if(endsWith("AT") || endsWith("BL") || endsWith("IZ"))
			data.append("E"); // replace doesn't work if new string will be longer than original
		else if(doublec())
		{
			ch = data[len-1];
			if (!(ch == 'L' || ch == 'S' || ch == 'Z'))
				data.replace(len-1,1,"");
		}
		else if(m(len)==1 && cvc(len-1))
		{
			data.append("E");
		}
	}
}

/*
	(*v*) Y -> I
*/
void Suffix_Step1c() 
{ 
	int len = data.size();
	if(endsWith("Y") && vowelInStem(len-1))
		data.replace(len-1,1,"I");
}

//Used in Step 2 and Step 3 to check if m()>0 and then replace
void r(string replace_string,int ends_len) 
{ 
	int len = data.size();
	if (m(len-ends_len) > 0) 
		data.replace(len-ends_len,ends_len,replace_string);
}

/* Step 2 maps double suffices to single ones. so -ization ( = -ize plus -ation) maps to -ize etc. note that the string before the suffix must give m() > 0. */
void Suffix_Step2() 
{
	int len = data.size();
	switch (data[len-2]) //Check for 2nd Last char
	{
		case 'A': if (endsWith("ATIONAL")){ r("ATE",7); break; } // 7 = length of "ATIONAL"
		          if (endsWith("TIONAL")) { r("TION",6); break; }
		          break;
		case 'C': if (endsWith("ENCI")) { r("ENCE",4); break; }
		          if (endsWith("ANCI")) { r("ANCE",4); break; }
		          break;
		case 'E': if (endsWith("IZER")) { r("IZE",4); break; }
		          break;
		case 'L': if (endsWith("ABLI")) { r("ABLE",4); break; }
		          if (endsWith("ALLI")) { r("AL",4); break; }
		          if (endsWith("ENTLI")) { r("ENT",5); break; }
		          if (endsWith("ELI")) { r("E",3); break; }
		          if (endsWith("OUSLI")) { r("OUS",5); break; }
		          break;
		case 'O': if (endsWith("IZATION")) { r("IZE",7); break; }
		          if (endsWith("ATION")) { r("ATE",5); break; }
		          if (endsWith("ATOR")) { r("ATE",4); break; }
		          break;
		case 'S': if (endsWith("ALISM")) { r("AL",5); break; }
		          if (endsWith("IVENESS")) { r("IVE",7); break; }
		          if (endsWith("FULNESS")) { r("FUL",7); break; }
		          if (endsWith("OUSNESS")) { r("OUS",7); break; }
		          break;
		case 'T': if (endsWith("ALITI")) { r("AL",5); break; }
		          if (endsWith("IVITI")) { r("IVE",5); break; }
		          if (endsWith("BILITI")) { r("BLE",6); break; }
		          break;
	} 
}

/* Step3 deals with -ic-, -full, -ness etc. similar strategy to step2. */
void Suffix_Step3() 
{
	int len = data.size();
	switch (data[len-1]) // Check for Last Char
	{
		case 'E': if (endsWith("ICATE")){ r("IC",5); break; }
		          if (endsWith("ATIVE")) { r("",5); break; }
		          if (endsWith("ALIZE")) { r("AL",5); break; }
		          break;
		case 'I': if (endsWith("ICITI")) { r("IC",5); break; }
		          break;
		case 'L': if (endsWith("ICAL")) { r("IC",4); break; }
		          if (endsWith("FUL")) { r("",3); break; }
		          break;
		case 'S': if (endsWith("NESS")) { r("",4); break; }
		          break;
	} 
}

//Used in Step 4 to check if m()>1 and then replace
void r4(string replace_string,int ends_len) 
{ 
	int len = data.size();
	if (m(len-ends_len) > 1) 
		data.replace(len-ends_len,ends_len,replace_string);
}

/* Step 4 takes off -ant, -ence etc., in context <c>vcvc<v>. */
void Suffix_Step4() 
{
	int len = data.size();
	switch (data[len-2]) // Check for 2nd Last Char
	{
		case 'A': if (endsWith("AL")){ r4("",2); break; }
		          break;
		case 'C': if (endsWith("ANCE")) { r4("",4); break; }
				  if (endsWith("ENCE")) { r4("",4); break; }
		          break;
		case 'E': if (endsWith("ER")){ r4("",2); break; }
		          break; 
		case 'I': if (endsWith("IC")){ r4("",2); break; }
		          break;           
		case 'L': if (endsWith("ABLE")) { r4("",4); break; }
		          if (endsWith("IBLE")) { r4("",4); break; }
		          break;
		case 'N': if (endsWith("ANT")) { r4("",3); break; }
		          if (endsWith("EMENT")) { r4("",5); break; }
		          if (endsWith("MENT")) { r4("",4); break; }
		          if (endsWith("ENT")) { r4("",3); break; }
		          break;          
		case 'O': if ((endsWith("ION")) && (data[len-4] == 'S' || data[len-4] == 'T')) { r4("",3); break; } //(m>1 and (*S or *T))
		          if (endsWith("OU")) { r4("",2); break; }
		          break;          
		case 'S': if (endsWith("ISM")) { r4("",3); break; }
		          break;
		case 'T': if (endsWith("ATE")) { r4("",3); break; }
				  if (endsWith("ITI")) { r4("",3); break; }
		          break;
		case 'U': if (endsWith("OUS")) { r4("",3); break; }
		          break;
		case 'V': if (endsWith("IVE")) { r4("",3); break; }
		          break;        
		case 'Z': if (endsWith("IZE")) { r4("",3); break; }
		          break;                                 
	} 
}

/*
	(m>1) E     -> 
    (m=1 and not *o) E ->
*/
void Suffix_Step5a()
{
	int temp,len = data.size();
	if (endsWith("E"))
	{
		temp = m(len-1);
		if((temp>1) || (temp == 1 && !cvc(len-2))) // len-2 cause string is 0 indexed
			data.replace(len-1,1,"");
	}
}

/*
	(m > 1 and *d and *L) -> single letter
*/
void Suffix_Step5b()
{
	int len = data.size();
	if (data[len-1] == 'L' && doublec() && m(len)>1)
	{
			data.replace(len-1,1,"");
	}
}

void SuffixRemoval()
{
	fstream ipfile,opfile;
	ipfile.open("StopWordsRemoved.txt",ios::in);
	opfile.open("SuffixRemoved.txt",ios::out);
	
	while(ipfile>>data)
	{
		Suffix_Step1a();
		Suffix_Step1b();
		Suffix_Step1c();
		Suffix_Step2();
		Suffix_Step3();
		Suffix_Step4();
		Suffix_Step5a();
		Suffix_Step5b();

		opfile<<data<<" ";
    }

    ipfile.close();
    opfile.close();
}

void EquivalentStemRemoval(string filename)
{
	fstream ipfile,opfile,tempfile,indexfile;
	int wordscounted = 0, i;
	string temp;
	
	indexfile.open("conflation_output/index.txt",fstream::in | fstream::out | fstream::app);
	ipfile.open("SuffixRemoved.txt",ios::in);
	tempfile.open("SuffixRemoved.txt",ios::in); // To check if word has appeared before 
	opfile.open("conflation_output/" + filename,ios::out);

	indexfile<<filename<<endl;
	indexfile.close();

	ipfile>>data;
	opfile<<data<<"\n";

	wordscounted = 1;

	while(ipfile>>data)
	{
		for(i=0;i<wordscounted;i++)
		{
			tempfile>>temp;
			if(temp == data)
				break;
		}
		if(i==wordscounted)
			opfile<<data<<"\n";
		tempfile.clear();
		tempfile.seekg(0,tempfile.beg);
		wordscounted++;
	}

	ipfile.close();
	tempfile.close();
	opfile.close();
}

void display_file(string filename)
{
	fstream ipfile;
	string temp;
	ipfile.open(filename,ios::in);

	while(ipfile>>temp)
	{
		cout<<temp<<" ";
	}

	ipfile.close();
}

void run_conflation(string inputfile){
	cout<<inputfile<<endl;
	StopWordRemoval(inputfile);
	cout<<"\n\nStep 1 - Stop Words Removed:\n"<<endl;
	display_file("StopWordsRemoved.txt");

	SuffixRemoval();
	cout<<"\n\nStep 2 - Suffix Stripping:\n";
	display_file("SuffixRemoved.txt");

	EquivalentStemRemoval(inputfile);
	cout<<"\n\nStep 3 - Removing Equivalent Stems:\n";
	display_file("conflation_output" + inputfile + ".txt");
}

int main()
{
	ofstream ofs;
	ofs.open("conflation_output/index.txt", ofstream::out | ofstream::trunc);
	ofs.close();
	
	fstream ipfile;
	ipfile.open("conflation_input/index.txt");
	string temp;
	while(ipfile >> temp){
		run_conflation(temp);
	}	
	/*
	cout<<"Enter Input File Name: ";
	cin>>inputfile;

	cout<<"\nInput File:\n";
	display_file(inputfile);

	StopWordRemoval(inputfile);
	cout<<"\n\nStep 1 - Stop Words Removed:\n";
	display_file("StopWordsRemoved.txt");

	SuffixRemoval();
	cout<<"\n\nStep 2 - Suffix Stripping:\n";
	display_file("SuffixRemoved.txt");

	EquivalentStemRemoval();
	cout<<"\n\nStep 3 - Removing Equivalent Stems:\n";
	display_file("FinalOutput.txt");

	cout<<endl;
	*/	
	return 0;
}

/* Output:
Enter Input File Name: Input.txt

Input File:
Conflation happens when the identities of two or more individuals, concepts, or places, sharing some characteristics of one another, seem to be a single identity, and the differences appear to become lost. In logic, it is the practice of treating two distinct concepts as if they were one, which produces errors or misunderstandings as a fusion of distinct subjects tends to obscure analysis of relationships which are emphasized by contrasts. However, if the distinctions between the two concepts appear to be superficial, intentional conflation may be desirable for the sake of conciseness and recall. 

Step 1 - Stop Words Removed:
CONFLATION HAPPENS IDENTITIES TWO INDIVIDUALS CONCEPTS PLACES SHARING CHARACTERISTICS SINGLE IDENTITY DIFFERENCES APPEAR LOST LOGIC PRACTICE TREATING TWO DISTINCT CONCEPTS PRODUCES ERRORS MISUNDERSTANDINGS FUSION DISTINCT SUBJECTS TENDS OBSCURE ANALYSIS RELATIONSHIPS EMPHASIZED CONTRASTS DISTINCTIONS TWO CONCEPTS APPEAR SUPERFICIAL INTENTIONAL CONFLATION DESIRABLE SAKE CONCISENESS RECALL 

Step 2 - Suffix Stripping:
CONFLAT HAPPEN IDENT TWO INDIVIDU CONCEPT PLACE SHARE CHARACTERIST SINGL IDENT DIFFER APPEAR LOST LOGIC PRACTIC TREAT TWO DISTINCT CONCEPT PRODUC ERROR MISUNDERSTAND FUSION DISTINCT SUBJECT TEND OBSCUR ANALYSI RELATIONSHIP EMPHAS CONTRAST DISTINCT TWO CONCEPT APPEAR SUPERFICI INTENT CONFLAT DESIR SAKE CONCIS RECAL 

Step 3 - Removing Equivalent Stems:
CONFLAT HAPPEN IDENT TWO INDIVIDU CONCEPT PLACE SHARE CHARACTERIST SINGL DIFFER APPEAR LOST LOGIC PRACTIC TREAT DISTINCT PRODUC ERROR MISUNDERSTAND FUSION SUBJECT TEND OBSCUR ANALYSI RELATIONSHIP EMPHAS CONTRAST SUPERFICI INTENT DESIR SAKE CONCIS RECAL 
*/

/* Output:
Enter Input File Name: Apples.txt

Input File:
Apple trees are large if grown from seed. Generally apple cultivars are propagated by grafting onto rootstocks, which control the size of the resulting tree. There are more than 7,500 known cultivars of apples, resulting in a range of desired characteristics. Different cultivars are bred for various tastes and uses, including cooking, eating raw and cider production. Trees and fruit are prone to a number of fungal, bacterial and pest problems, which can be controlled by a number of organic and non-organic means. In 2010, the fruit's genome was sequenced as part of research on disease control and selective breeding in apple production. 

Step 1 - Stop Words Removed:
APPLE TREES LARGE GROWN SEED GENERALLY APPLE CULTIVARS PROPAGATED GRAFTING ROOTSTOCKS CONTROL SIZE RESULTING TREE KNOWN CULTIVARS APPLES RESULTING RANGE DESIRED CHARACTERISTICS DIFFERENT CULTIVARS BRED VARIOUS TASTES USES INCLUDING COOKING EATING RAW CIDER PRODUCTION TREES FRUIT PRONE NUMBER FUNGAL BACTERIAL PEST PROBLEMS CONTROLLED NUMBER ORGANIC NON ORGANIC MEANS FRUIT S GENOME SEQUENCED PART RESEARCH DISEASE CONTROL SELECTIVE BREEDING APPLE PRODUCTION 

Step 2 - Suffix Stripping:
APPL TREE LARG GROWN SEED GENER APPL CULTIVAR PROPAG GRAFT ROOTSTOCK CONTROL SIZE RESULT TREE KNOWN CULTIVAR APPL RESULT RANG DESIR CHARACTERIST DIFFER CULTIVAR BRED VARIOU TAST USE INCLUD COOK EAT RAW CIDER PRODUCT TREE FRUIT PRONE NUMBER FUNGAL BACTERI PEST PROBLEM CONTROL NUMBER ORGAN NON ORGAN MEAN FRUIT GENOM SEQUENC PART RESEARCH DISEAS CONTROL SELECT BREED APPL PRODUCT 

Step 3 - Removing Equivalent Stems:
APPL TREE LARG GROWN SEED GENER CULTIVAR PROPAG GRAFT ROOTSTOCK CONTROL SIZE RESULT KNOWN RANG DESIR CHARACTERIST DIFFER BRED VARIOU TAST USE INCLUD COOK EAT RAW CIDER PRODUCT FRUIT PRONE NUMBER FUNGAL BACTERI PEST PROBLEM ORGAN NON MEAN GENOM SEQUENC PART RESEARCH DISEAS SELECT BREED
*/
