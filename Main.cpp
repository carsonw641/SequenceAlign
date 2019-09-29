#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

typedef struct Node {
    Node* prevNode;

    int x, y, z;
    int weight;
    
    Node(Node* prevNode, int weight, int x, int y, int z): prevNode(prevNode), weight(weight), x(x), y(y), z(z){}
    
    ~Node() {
        x = y = z = weight = 0;
        prevNode = nullptr;
    }

} Node;

void traverseMap(std::ifstream& stream, std::string& s1, std::string& s2, std::vector<std::vector<std::vector<Node*>>>& map, std::vector<char> charSequence){
    int minPathVal = std::numeric_limits<int>::min();
    
    int column = map.size()-1, row = charSequence.size();
    int off_set = column + 1;
    /* DETERMINE ENDING NODE */
    Node* iter = nullptr;

    for (int i = 0; i < 3; i++){
        if (map[column][row][i]->weight > minPathVal){
            iter = map[column][row][i];
            minPathVal = iter->weight;
        }
    }

    std::cout << "Minimum alignment cost: " << iter->weight << std::endl;
    char tempChar;
    while (iter ->prevNode != nullptr){
        if (iter-> z == 0){
            stream.seekg(column - off_set , std::ios::end);
            if (stream.peek() == '\n'){
                stream >> tempChar;
                column--;
                stream.seekg(column - off_set , std::ios::end);
            }
            stream >> tempChar;

            s1.insert(s1.begin(), charSequence[row-1]);
            s2.insert(s2.begin(), tempChar);

            column--;
            row--;
        }else if (iter->z == 1){
            // s2 gets - sign
            s1.insert(s1.begin(), charSequence[row-1]);
            s2.insert(s2.begin(), '-');

            row--;
        }else {
            //s1 gets - sign
            stream.seekg(column - off_set, std::ios::end);
            if (stream.peek() == '\n'){
                stream >> tempChar;
                column--;
                stream.seekg(column - off_set , std::ios::end);
            }
            stream >> tempChar;

            s1.insert(s1.begin(), '-');
            s2.insert(s2.begin(), tempChar);

            column--;
        }

        iter = iter->prevNode;
    }
}

void traverseLocalMap(std::string& substring, std::vector<std::vector<std::vector<Node*>>>& map, std::vector<char> charSequence){
    int greatestValue = 0;
    Node* iter = nullptr;

    for (int col = 0; col < map.size(); col++){
        for (int row = 0; row < map[col].size(); row++){
            if (map[col][row][0]->weight > greatestValue){
                iter = map[col][row][0];
                greatestValue = iter->weight;
            }
        }
    }

    while (iter != nullptr && map[iter->x][iter->y][0] > 0){ 
        substring.insert(substring.begin(), charSequence[iter->y-1]);
        iter = iter->prevNode;
    }
}

void determineGapPenalties(std::vector<std::vector<std::vector<Node*>>>& map, int column, int row, int match, int misMatch, int gap, int startGap, bool matches) {
    
    /* ----------------------------- Determine M ---------------------------------- */

    Node* prevPoint = nullptr;
    int maxVal = std::numeric_limits<int>::min();
    
    for (int i = 0; i < 3; i++){
        if (map[column-1][row-1][i] -> weight > maxVal){
            prevPoint = map[column-1][row-1][i];
            maxVal = prevPoint->weight;
        }
            
    }
    if (matches){
        map[column][row][0] = new Node(prevPoint, maxVal + match, column, row, 0);
    } else {
        map[column][row][0] = new Node(prevPoint, maxVal + misMatch, column, row, 0);
    }
    
    /* ----------------------------- Determine X --------------------------------- */
    
    prevPoint = nullptr;
    maxVal = std::numeric_limits<int>::min();
    
    for (int i = 0; i < 3; i++){
        if (map[column][row-1][i]-> weight > maxVal){
            if (i == 1){
                if (map[column][row-1][i]->weight + gap > maxVal){
                    prevPoint = map[column][row-1][i];
                    maxVal = prevPoint->weight + gap;
                }
            }else {
                if (map[column][row-1][i]->weight + gap + startGap > maxVal){
                    prevPoint = map[column][row-1][i];
                    maxVal = prevPoint -> weight + startGap + gap;
                }
            }
        }
    }

    map[column][row][1] = new Node(prevPoint, maxVal, column, row, 1);

    /* ---------------------------- Determine Y ------------------------------------ */
    
    prevPoint = nullptr;
    maxVal = std::numeric_limits<int>::min();

    for (int i = 0; i < 3; i++){
        if (map[column-1][row][i]->weight > maxVal){
            if (i == 2){
                if (map[column-1][row][i]->weight + gap > maxVal){
                    prevPoint = map[column-1][row][i];
                    maxVal = prevPoint->weight + gap;
                }
            }else {
                if (map[column-1][row][i]->weight + gap + startGap > maxVal){
                    prevPoint = map[column-1][row][i];
                    maxVal = prevPoint -> weight + startGap + gap;
                }
            }
        }
    }

    map[column][row][2] = new Node(prevPoint, maxVal, column, row, 2);

}

void determineLocalGapPenalties(std::vector<std::vector<std::vector<Node*>>>& map, int column, int row, int match, int misMatch, int gap, int startGap, bool matches){
    /* ----------------------------- Determine M ---------------------------------- */

    Node* prevPoint = nullptr;
    int maxVal = 0;
    
    for (int i = 0; i < 3; i++){
        if (map[column-1][row-1][i] -> weight > maxVal){
            prevPoint = map[column-1][row-1][i];
            maxVal = prevPoint->weight;
        }
            
    }

    if (matches){
        map[column][row][0] = new Node(prevPoint, maxVal + match, column, row, 0);
    } else {
        if (maxVal + misMatch > 0 ){
            map[column][row][0] = new Node(prevPoint, maxVal + misMatch, column, row, 0);
        }else {
            map[column][row][0] = new Node (nullptr, 0, column, row, 0);
        }
    }
    
    /* ----------------------------- Determine X --------------------------------- */
    
    prevPoint = nullptr;
    maxVal = 0;
    
    for (int i = 0; i < 3; i++){
        if (i == 1){
            if (map[column][row-1][i]->weight + gap > maxVal){
                prevPoint = map[column][row-1][i];
                maxVal = prevPoint->weight + gap;
            }
        }else {
            if (map[column][row-1][i]->weight + gap + startGap > maxVal){
                prevPoint = map[column][row-1][i];
                maxVal = prevPoint -> weight + startGap + gap;
            }
        }
    }
    

    map[column][row][1] = new Node(prevPoint, maxVal, column, row, 1);

    /* ---------------------------- Determine Y ------------------------------------ */
    
    prevPoint = nullptr;
    maxVal = 0;

    for (int i = 0; i < 3; i++){
        if (i == 2){
            if (map[column-1][row][i]->weight + gap > maxVal){
                prevPoint = map[column-1][row][i];
                maxVal = prevPoint->weight + gap;
            }
        }else {
            if (map[column-1][row][i]->weight + gap + startGap > maxVal){
                prevPoint = map[column-1][row][i];
                maxVal = prevPoint -> weight + startGap + gap;
            }
        }
    }

    map[column][row][2] = new Node(prevPoint, maxVal, column, row, 2);
}

void initializeMap(std::vector<std::vector<std::vector<Node*>>>& map, int rows, int gap, int startGap){
    map.push_back(std::vector<std::vector<Node*>>(
            rows +1, std::vector<Node*>(3, nullptr)
        )
    );

    // Initial index 0, 0
    map[0][0][0] = new Node(nullptr, 0, 0, 0, 0);
    map[0][0][1] = new Node(nullptr, startGap, 0, 0, 1);
    map[0][0][2] = new Node(nullptr, startGap, 0, 0, 2);

    for (int row = 1; row < rows + 1; row++){
        map[0][row][0] = new Node(nullptr,std::numeric_limits<int>::min(), 0, row, 0);
        map[0][row][1] = new Node(map[0][row-1][1], startGap + gap*row, 0, row, 1);
        map[0][row][2] = new Node(nullptr, std::numeric_limits<int>::min(), 0, row, 2);
    }
}

void initializeLocalMap(std::vector<std::vector<std::vector<Node*>>>& map, int rows){
    map.push_back(std::vector<std::vector<Node*>>(
            rows +1, std::vector<Node*>(3, nullptr)
        )
    );

    for (int row = 0; row < rows + 1; row++){
        for (int z = 0; z < 3; z++){
            map[0][row][z] = new Node(nullptr, 0, 0, row, z);
        }
    }
}

void setInitialColumnNode(std::vector<std::vector<std::vector<Node*>>>& map, int column, int gap, int startGap){
    map[column][0][0] = new Node(nullptr, std::numeric_limits<int>::min(), column, 0, 0);
    map[column][0][1] = new Node(nullptr, std::numeric_limits<int>::min(), column, 0, 1);
    map[column][0][2] = new Node(map[column-1][0][2], startGap + column * gap, column, 0, 2);
}

void setInitialLocalColumnNode(std::vector<std::vector<std::vector<Node*>>>& map, int column){
    for (int z = 0; z < 3; z++){
        map[column][0][z] = new Node(nullptr, 0, column, 0, z);
    }
}

int main () {
    
    /* PREDEFINED FILES IN SAME DIRECTORY USED FOR CONFIGURATION AND STRING SEQUENCE DATA */
    std::string configFile = "parameters.config", inputFile = "input.fasta";
    
    int match, gap, mismatch, h;


    /* PARAMETERS CONFIGURATION FILE PARSING AND VARIABLE ASSIGNMENT */
    
    std::ifstream config(configFile, std::ios::binary);

    if (!config.is_open()){
        std::cerr << "Failed to open file: " << configFile << std::endl;
        return EXIT_FAILURE;
    }else {
        std::string key;
        int weight;
        while (!config.eof()){
            config >> key >> weight;

            if (key == "match"){
                match = weight;
            }else if (key == "g") {
                gap = weight;
            }else if (key == "mismatch"){
                mismatch = weight;
            }else if (key == "h"){
                h = weight;
            }else {
                std::cerr << "Invalid config key: " << key << std::endl;
                return EXIT_FAILURE;
            }
        }
    }

    config.close();

    /* END OF PARAMTERS CONFIGURATION FILE OPERATIONS */

    std::vector<char> charSequence;
    std::vector<std::vector<std::vector<Node*>>> nodeMap;
    std::vector<std::vector<std::vector<Node*>>> localNodeMap;
    /* ------------------------------------------ */

    std::ifstream input(inputFile, std::ios::binary);

    if (!input.is_open()){
        std::cerr << "Failed to open file: " << inputFile << std::endl;
        return EXIT_FAILURE;
    }else {
        char tempChar;
        bool firstSequence = false;

        while (input >> tempChar) {
            if (tempChar == '>' && !firstSequence){
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                firstSequence = true;
            }else if (tempChar == '>' && firstSequence){
                input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
            }else {
                charSequence.push_back(tempChar);
            }
        }

        initializeMap(nodeMap, charSequence.size(), gap, h);
        initializeLocalMap(localNodeMap, charSequence.size());

        int column = 0;

        while(input >> tempChar){
            nodeMap.push_back(
                std::vector<std::vector<Node*>>(
                    charSequence.size() + 1,
                    std::vector<Node*>(3, nullptr)
                )
            );

            localNodeMap.push_back(
                std::vector<std::vector<Node*>>(
                    charSequence.size() + 1,
                    std::vector<Node*>(3, nullptr)
                )
            );

            column = nodeMap.size()-1;
            
            setInitialColumnNode(nodeMap, column, gap, h);
            setInitialLocalColumnNode(localNodeMap, column);

            for (int row = 1; row < charSequence.size() + 1; row++){
                determineGapPenalties(
                    nodeMap, 
                    column, 
                    row, 
                    match, 
                    mismatch, 
                    gap, 
                    h,
                    (charSequence[row-1] == tempChar));
            }

            for (int row = 1; row < charSequence.size() + 1; row++){
                determineLocalGapPenalties(
                    localNodeMap, 
                    column, 
                    row, 
                    match, 
                    mismatch, 
                    gap, 
                    h,
                    (charSequence[row-1] == tempChar));
            }

        }
        input.clear();
        std::string s1, s2, substring;
        
        std::cout << "Scores: match = " << match << " mismatch = " << mismatch 
                << " h = " << h << " g = " << gap << '\n' << std::endl;
        std::cout << "Sequence 1 length = " << charSequence.size() << std::endl;
        std::cout << "Sequence 2 length = " << nodeMap.size() - 1 << '\n' << std::endl;


        traverseMap(input, s1, s2, nodeMap, charSequence);
        traverseLocalMap(substring, localNodeMap, charSequence);

        std::cout << "Global Alignment sequences:" << std::endl;

        std::cout << "s1:\n" << s1 << "\ns2:\n"  << s2 << '\n' << std::endl;
        std::cout << "Local alignment substring:\n" << substring << std::endl;
    }

    input.close();

    /* --------------------------------------------- */

    /* CLEARING CELL MAP DATA VALUES SINCE WE ALLOCATE RAW POINTERS */
    for (std::vector<std::vector<Node*>>& x: nodeMap){
        for (std::vector<Node*>& y: x){
            for (Node* z: y){
                delete z;
                z = nullptr;
            }
        }
    }

    for (std::vector<std::vector<Node*>>& x: localNodeMap){
        for (std::vector<Node*>& y: x){
            for (Node* z: y){
                delete z;
                z = nullptr;
            }
        }
    }

    return 0;
}