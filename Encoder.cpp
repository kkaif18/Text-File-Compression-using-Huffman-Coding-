#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

//mapping all the characters in the input string to their frequencies
unordered_map<char,int> freq;

//defining a node that stores the character and its frequency
struct Node {
    char ch;
    int freq;
    Node *left, *right;

    //Variable to use for tie-breaking
    char minChar; 

    Node(char c, int f, Node* l = nullptr, Node* r = nullptr) {
        ch = c;
        freq = f;
        left = l;
        right = r;

        // If it's a leaf node, minChar is just the character itself
        if (left == nullptr && right == nullptr) {
            minChar = c;
        } 
        // If it's an internal node, pick the smallest char from its children
        else {
            minChar = (l->minChar < r->minChar) ? l->minChar : r->minChar;
        }
    }
};

// Logical Structure Operator that helps to change the default logic of the priority Queue( making the min heap that is, the smallest element is at the top of the tree)
struct Compare {
    bool operator()(Node* l, Node* r) {
        // Primary Sort: By Frequency (Low to High)
        if (l->freq != r->freq) {
            return l->freq > r->freq;
        }
        
        // Secondary Sort (Tie-Breaker): By Character (A before B)
        return l->minChar > r->minChar;
    }
};
// the function that provides the generated Code, more like implementing the huffman Coding Algorithm
void generateCodes(Node* root, string str, unordered_map<char, string> &huffmanCode){
    // base case
    if(root == NULL) return;
    // When a leaf Node is found, we can assign the string to it
    if(root->left == NULL && root->right == NULL){
        huffmanCode[root->ch] = str;
    }
    //when we move left in the tree a zero is added to the string, while a one is added for the right side
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

string buildHuffmanTree(string text){
    
    for(char ch : text){
        freq[ch]++;
    }

    cout << "---MAP---" << endl;
    for (auto pair : freq) {
        cout << "'" << pair.first << "' : " << pair.second << endl;
    }
    cout << "---------" << endl;
    // implementating a priority queue which stores Node pointers, here "vector<Node*>" is just a container for the queue
    // priority queue is a heap tree , but the computer can't process a tree natively , it uses vector as a continuous memory allocation
    // we added the Compare as function based on which we can generate a min heap rather than the default max heap
    priority_queue<Node*, vector<Node*>, Compare> pq;

    //filling up the queue
    for (auto pair : freq) {
        // Create a new node for every character found in the map
        pq.push(new Node(pair.first, pair.second));
    }
    // actually building of the huffman tree
    while(pq.size() != 1){
        //taking out the two characters with the least frequencies 
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        // making a new Node that takes the sum of the two frequencies taken earlier
        // this node is just the internal node of the huffman tree, so here the '$' placeholder acts as a dummy as , this internal node doesn't require any character in it
        Node* sum = new Node('$', left->freq + right->freq);

        // making connections
        sum->left = left;
        sum->right = right;

        pq.push(sum);
    }

    // the reamining is the sum of all the frequencies i.e. the length of the string, it acts as the root of the Tree
    Node*root = pq.top();

    // Creating a map that stores the assigned compressed code to each character
    unordered_map<char, string> huffmanCode;
    // running the function that generates the code for each char
    // the string "" will traverse through each node and assign a unique code to leaf node i.e when a character is met
    generateCodes(root, "", huffmanCode);

    cout<<"\nHuffman Codes are:\n"<<endl;

    //displaying the generated code for each character
    for(auto pair : huffmanCode){
        cout<< pair.first <<" "<<pair.second<<endl;
    }

    //Creating the Compressed Binary String as for the input string
    string s = "";
    for(char ch : text){
        s += huffmanCode[ch];
    }
    // Printing the result(the compressed code the input string)
    return s;

}
//function to take out the file as a string 
string readFile(string filename){
    //here ifstream is like a file reader which stores the filename file into the object inFile
    ifstream inFile(filename);
    //we also dont use getline to take the file as a input as it creates issues over new lines, which lefts out data, which may create the data corrupted and lost,huffman coding is for lossless compression, on single byte is missed
    //check for valid file
    if(!inFile){
        //cerr is faster than cout as it prints instantly as the error is hit, unlike cout(console output) which waits fo the whole data to process to then print
        cerr<<"Error: Invalid File "<< filename <<endl;
        exit(1);
    }

    //stringstream helps to take the characters as a stream as we can't directly put the characters into a string
    stringstream ss;
    //rd.buf() takes out raw block of memory as put it into the ss as it doesn't mind of new lines, tabs, spaces. Making it much faster than ifstream which actually checks the formatting of each character which is quite inefficient 
    ss << inFile.rdbuf();
    
    //str() converts the stringstream to a string
    return ss.str();
}

void saveFile(string encodedString, string output){
    //ios::binary opens the file in binary mode, it takes raw data only as it is, does't interpruts 10(byte as a new line) unlike the default textmode we raeds 10 as \n and adds \r(return carriage as autocorrection)
    ofstream outFile(output, ios::binary);

    int size = (int)freq.size();
    //we typecasted the freq size because freq.size() gives the size in size_t(size type), it may differ from old(4 bytes) to new(8 bytes), hence to make it common across the all computers we made it int
    //we need to send the size first, to make sure the decoder knows how many of the chsr,freq pairs to be read while decompression
    outFile.write((char*)&size, sizeof(size));
    //the write function is a tool(it reads addresses only, it accessed the values throught their address, no actual value is passed on to it) to dump raw bytes into the outFile, since the &size is a int address, the writer can't process the integer type, to solve this , we use the character pointer as a disguise to the writer and just see the size as a character and create no interference.
    // we also send the sizeof(size) to make sure how many bytes is to be copied(it is necessary for the writer to know how much to copy as to when to end)

    //Now we just copy the pairs of charcters and frequencies just like above
    for(auto pair : freq){

        char ch = pair.first;
        int fq = pair.second;

        outFile.write((char*)&ch, sizeof(ch));
        outFile.write((char*)&fq, sizeof(fq));
    }

    //We use an unsigned char type to take values from 0 to 255 as huffman codes can be maximum to 255. We do not use signed because it takes negative number into consideration we makes the compression corrupted in huffman coding
    unsigned char buffer = 0;
    int count = 0;

    //looping over the input encoded string taking 8 bits together to form a byte and put in the Outfile
    for(char bit : encodedString){

        //moving the bit by one place to the left
        // e.g. 00000001 changes to 00000010
        buffer = buffer << 1;
        //once we have shifted the place we check if the next bit is 1 we can add that 1 or just leave it as it is for 0 ,at the leftmost bit as the shift operator has given an empty place
        if( bit == '1') buffer = buffer | 1;

        // increment at counting
        count++;

        //when all the 8 bits are carried in the buffer we try to put that one byte in the outfile
        if(count == 8){
            // put just put raw bytes as it is in the outFile
            outFile.put(buffer);

            //reseting the count and buffer for the next byte
            count = 0;
            buffer = 0;
        }
    }
    
    //checking the edge case when the last buffer carriage has taken less than 8 bits at the end
    //eg. if the last bits are 101 but the buffer at the end of the loop will be 00000101 which can't be directly put in the outFile as it alters the data compression, so we shift it by (8 - count) times so it makes it 10100000, so the output file will now raed it as 101 and leave the padding(receding zeroes)
    if(count > 0){
        buffer = buffer << (8 - count);
        outFile.put(buffer);
    }

    // command to finish of all the buffers stored in some temporary array to write in the actual disk
    outFile.close();

    cout<< "\nCompressed data written to "<< output << endl;
}

int main(){
    //taking the name of the text file as a string 
    string filename;
    cout<<"Enter the File Name: ";
    getline(cin, filename);
    
    string content = readFile(filename);

    if(content.empty()){
        cout<< "The file is Empty!\n"<< endl;
        return 0;
    }

    cout<< "The file has "<<content.length()<< " characters\n";
    string s = buildHuffmanTree(content);
    
    saveFile(s, "compressed.bin");
   
    ifstream originalFile("sample.txt", ios::binary | ios::ate);
    ifstream compressedFile("compressed.bin", ios::binary | ios::ate);

    if (originalFile && compressedFile) {
        // 'tellg()' returns the current position. Since we opened with 'ate' (At The End),
        // it tells us the total file size in bytes.
        streampos originalSize = originalFile.tellg();
        streampos compressedSize = compressedFile.tellg();

        cout << "\n==========================================" << endl;
        cout << "         COMPRESSION STATISTICS           " << endl;
        cout << "==========================================" << endl;
        cout << "Original Size:   " << originalSize << " bytes" << endl;
        cout << "Compressed Size: " << compressedSize << " bytes" << endl;

        // Calculate saving percentage
        double saving = (1.0 - ((double)compressedSize / (double)originalSize)) * 100;
        
        cout << "Space Saved:     " << saving << "%" << endl;
        cout << "==========================================" << endl;
    } else {
        cout << "Error checking file sizes." << endl;
    }

    originalFile.close();
    compressedFile.close();
    return 0;
}
