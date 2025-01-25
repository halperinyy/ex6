#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

# define INT_BUFFER (128)
# define MAX_POKE_ID (151)
# define MIN_POKE_ID (1)

// #define YAAKOV_DEBUG
int validate_id = 0;

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    
#ifdef YAAKOV_DEBUG
    printf("YAAKOV: readIntSafe:: value = %d\n",value);
#endif

    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

#ifdef  YAAKOV_DEBUG
    printf("YAAKOV: getDynamicInput:: input = %s\n",input);
#endif
    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (node == NULL)
    {
        return;
    }
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    ValidateAll();

    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
    ValidateAll();

}

void FreeNode(PokemonNode* node)
{
    if(node)
    {
        if(node->data)
        {
            free(node->data);
        }
        free(node);
    }
}

void displayAlphabetical(PokemonNode *root)
{
    //creating the 1st
    PokemonNode* abTreeROOT = NULL;
    if(root)
    {
        //a function copyng all the original BST to the new one
        abTreeROOT = CreateAlphabetTreeFromTree(abTreeROOT, root);
    }
    inOrderTraversal(abTreeROOT);
    //After displaying, we can free the tree.
    freeTree(abTreeROOT);
}

PokemonNode* CreateAlphabetTreeFromTree(PokemonNode* abroot, PokemonNode* src_node)
{
    if(src_node == NULL)
    {
        return NULL;
    }

    PokemonNode* newnode = allocTreeNode(src_node->data->id);
    if(abroot == NULL)
    {
        abroot = newnode;
    }
    else
    {
        addNodeToABTree(abroot, newnode);
    }
    
    CreateAlphabetTreeFromTree(abroot, src_node->left);
    CreateAlphabetTreeFromTree(abroot, src_node->right);

    return abroot;
}

void addToABtree(PokemonNode* abroot)
{
    PokemonNode* abTreeNode = allocTreeNode(abroot->data->id);
    addNodeToABTree(abroot, abTreeNode);
}

void addNodeToABTree(PokemonNode *owner, PokemonNode *newNode)
{
    //Case that the tree is empty
    if(owner == NULL)
    {
        owner = newNode; 
    }
    //There is some root node
    else
    {
        PokemonNode* CurrNode = owner;
        PokemonNode* NextNode = owner;

        while(NextNode != NULL)
        {
            CurrNode = NextNode;
            NextNode = (strcmp(newNode->data->name, CurrNode->data->name) < 0) ? CurrNode->left : CurrNode->right;
        }

        if(strcmp(newNode->data->name, CurrNode->data->name) < 0)
        {
            CurrNode->left = newNode;
        }
        else
        {
            CurrNode->right = newNode;
        }
    }
}

void postOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
    {
        return;
    }
    //recursion to the left
    postOrderTraversal(root->left);
    //recursion to the right
    postOrderTraversal(root->right);
    printPokemonNode(root);
}

void inOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
    {
        return;
    }
    //recursion to the left
    inOrderTraversal(root->left);
    printPokemonNode(root);
    //recursion to the right
    inOrderTraversal(root->right);
}

void preOrderTraversal(PokemonNode *root)
{
    if(root == NULL)
    {
        return;
    }
    printPokemonNode(root);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}

int tree_height(PokemonNode* root) 
{
    // Get the height of the tree
    if(root == NULL)
        return 0;
    else {
        // Find the height of both kids, and we'll use the higher one
        int left_height = tree_height(root->left);
        int right_height = tree_height(root->right);
        if (left_height >= right_height)
            return left_height + 1;
        else
            return right_height + 1;
    }
}


void displayBFS(PokemonNode* tree)
{
    int height = tree_height(tree);
    if(tree == NULL)
    {
        return;
    }
    for (int i=0; i <= height; i++)
    {
        print_level(tree, i);
    }
}

void print_level(PokemonNode* root, int height)
{
    if(root == NULL)
    {
        return;
    }

    if(height == 0) 
    {
        printPokemonNode(root);
    }
    else 
    {
        print_level(root->left, height - 1);
        print_level(root->right, height - 1);
    }

}
void ValidateNode(PokemonNode* node)
{
    assert(node->data);
    assert(node->data->hp > 0);
    assert(node->data->attack < 200);
}

void ValidateTree(PokemonNode* node)
{
    if(node == NULL)
    {
        return;
    }

    ValidateNode(node);

    ValidateTree(node->left);
    ValidateTree(node->right);
}

void ValidateAll(void)
{
    OwnerNode *owner = ownerHead;

    if(owner)
    {
        do
        {
            ValidateTree(owner->pokedexRoot);
            owner = owner->next;
        } while(owner != NULL && owner->next != ownerHead);
    }
    
    validate_id++;
}
// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    ValidateAll();
    if(ownerHead == NULL)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");


    printf("1. %s\n", ownerHead->ownerName);
    OwnerNode* currNode = ownerHead;
    OwnerNode* nextNode = currNode->next;
    int index = 0;
    while(nextNode != ownerHead && nextNode != NULL)
    {
        printf("%d. %s\n", index + 2, nextNode->ownerName);
        currNode = nextNode;
        nextNode = nextNode->next;
        index++;
    }

    ValidateAll();


    currNode = ownerHead;
    int choice = readIntSafe("Choose a Pokedex by number: ") - 1;
    for(int i = 0; i < choice; i++)
    {
        currNode = currNode->next;
    }
    printf("\nEntering %s's Pokedex...\n", currNode->ownerName);
    int subchoice;
    OwnerNode* curOwner = currNode;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", curOwner->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");
        subchoice = readIntSafe("Your choice: ");
        switch (subchoice)
        {
        case 1:
            addPokemon(curOwner);
            break;
        case 2:
            displayMenu(curOwner);
            break;
        case 3:
            freePokemon(curOwner);
            break;
        case 4:
            pokemonFight(curOwner);
            break;
        case 5:
            evolvePokemon(curOwner);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
        ValidateAll();
    } while (subchoice != 6);
}

void evolvePokemon(OwnerNode *owner)
{
    if((owner->pokedexRoot == NULL))
    {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    int inputpoki = readIntSafe(" ");

    if(inputpoki < MIN_POKE_ID || inputpoki > MAX_POKE_ID)
    {
        printf("No Pokemon with ID %d found.\n", inputpoki);
    }

    if(findInTree(owner->pokedexRoot, inputpoki))
    {
        //todo: remove our poke 
        freePokemon2(owner, inputpoki);
        printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", pokedex[inputpoki - 1].name, pokedex[inputpoki - 1].id , pokedex[inputpoki].name, pokedex[inputpoki].id);
    } else {
        int id = inputpoki + 1;
        //todo: remove our poke 
        freePokemon2(owner, inputpoki);
        PokemonNode *newtreenode = allocTreeNode(id);
        addNodeToTree(owner, newtreenode);
        printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", pokedex[inputpoki - 1].name, pokedex[inputpoki - 1].id , pokedex[inputpoki].name, pokedex[inputpoki].id);
    }
}

PokemonNode* findInTree(PokemonNode* root, int id)
{
    if (root == NULL)
    {
        return NULL;
    }
    if (root->data->id > id)
    {
        return findInTree(root->left, id);
    }
    if (root->data->id < id)
    {
        return findInTree(root->right, id);
    }
    return root;
}

PokemonNode* findFatherInTree(PokemonNode* root, PokemonNode* kid, int id)
{
    if (root == NULL)
    {
        return NULL;
    }
    if(root->left == kid || root->right == kid)
    {
        return root;
    }
    if (root->data->id > id)
    {
        return findFatherInTree(root->left, kid, id);
    }
    if (root->data->id < id)
    {
        return findFatherInTree(root->right, kid, id);
    }
    return root;
}

void pokemonFight(OwnerNode *owner){
    if(owner->pokedexRoot == NULL)
    {
        printf("Pokedex is empty.\n");
        return;
    }
    int idinput1 = readIntSafe("Enter ID of the first Pokemon: ");
    int idinput2 = readIntSafe("Enter ID of the second Pokemon: ");
    // for(int i = MIN_POKE_ID; i <= MAX_POKE_ID; i++)
    // {
    //     if((idinput1 = i))
    //     {
    //         idinput1checker++;
    //     }
    // }
    // for(int i = MIN_POKE_ID; i <= MAX_POKE_ID; i++)
    // {
    //     if((idinput2 = i))
    //     {
    //         idinput2checker++;
    //     }
    // }

    if(findInTree(owner->pokedexRoot, idinput1) && findInTree(owner->pokedexRoot, idinput2))
    {
        float totalpower1 = 1.5 * pokedex[idinput1 - 1].attack + 1.2 * pokedex[idinput1 - 1].hp;
        float totalpower2 = 1.5 * pokedex[idinput2 - 1].attack + 1.2 * pokedex[idinput2 - 1].hp;
        printf("Pokemon 1: %s (Score = %.2f)\n", pokedex[idinput1 - 1].name, totalpower1);
        printf("Pokemon 2: %s (Score = %.2f)\n", pokedex[idinput2 - 1].name, totalpower2);

        if(totalpower1 >= totalpower2)
        {
            if(totalpower1 == totalpower2)
            {
            printf("It's a tie!\n");
            }
            else
            {
                printf("%s wins!\n", pokedex[idinput1 - 1].name);
            }
        }
        else
        {
            printf("%s wins!\n", pokedex[idinput2 - 1].name);
        }
    }
    else
    {
        printf("One or both Pokemon IDs not found.\n");
    }

}

void freePokemon2(OwnerNode *input, int id)
{
    ValidateAll();
    PokemonNode* inputpokemon = findInTree(input->pokedexRoot, id);
    ValidateAll();

    if(!inputpokemon)
    {
        printf("No Pokemon with ID %d found.\n", id);
    }
    else
    {
        if((inputpokemon->left != NULL) && (inputpokemon->right != NULL))
        {
            PokemonNode* fatherofpokemon = findFatherInTree(input->pokedexRoot, inputpokemon, id);
            if(fatherofpokemon->left == inputpokemon)
            {
                inputpokemon = remover(inputpokemon);
                fatherofpokemon->left = inputpokemon;
            }
            else
            {
                inputpokemon = remover(inputpokemon);  
                fatherofpokemon->left = inputpokemon;
            }

        }
        else
        {
            if((inputpokemon->left != NULL) || (inputpokemon->right != NULL))
            {
                PokemonNode* fatherofpokemon = findFatherInTree(input->pokedexRoot, inputpokemon, id);
                if(fatherofpokemon->right == inputpokemon)
                {
                    fatherofpokemon->right = (inputpokemon->right) ? inputpokemon->right : inputpokemon->left;
                }
                else
                {
                    fatherofpokemon->left = (inputpokemon->right) ? inputpokemon->right : inputpokemon->left;
                }
            }
            else
            {
                PokemonNode* fatherofpokemon = findFatherInTree(input->pokedexRoot, inputpokemon, id);
                if(fatherofpokemon == inputpokemon)
                {
                    FreeNode(inputpokemon);
                    input->pokedexRoot = NULL;
                    return;
                }
                fatherofpokemon->left = (fatherofpokemon->left == inputpokemon) ? NULL : fatherofpokemon->left;
                fatherofpokemon->right = (fatherofpokemon->right == inputpokemon) ? NULL : fatherofpokemon->right;
            }
            FreeNode(inputpokemon);
            // free(inputpokemon->data);
            // free(inputpokemon->left);
            // free(inputpokemon->right);
        }
    } 
    ValidateAll();

}

void freePokemon(OwnerNode *input)
{
    if((input->pokedexRoot == NULL))
    {
        printf("No Pokemon to release.\n");
        return;
    }
    //input
    int id = readIntSafe("Enter Pokemon ID to release: ");
    freePokemon2(input, id);
    printf("Removing Pokemon %s (ID %d).\n", pokedex[id - 1].name, id);
}

PokemonNode* remover(PokemonNode* inputNode)
{
    if(inputNode == NULL)
    {
        return NULL;
    }

    if((inputNode->right == NULL) && (inputNode->left == NULL))
    {
        FreeNode(inputNode);
        return NULL;
    }
    if((inputNode->left) && !inputNode->right)
    {
        inputNode->data->attack = inputNode->left->data->attack;
        inputNode->data->CAN_EVOLVE = inputNode->left->data->CAN_EVOLVE;
        inputNode->data->hp = inputNode->left->data->hp;
        inputNode->data->id = inputNode->left->data->id;
        inputNode->data->name = inputNode->left->data->name;
        inputNode->data->TYPE = inputNode->left->data->TYPE;        
    }
    else
    {
    inputNode->data->attack = inputNode->right->data->attack;
    inputNode->data->CAN_EVOLVE = inputNode->right->data->CAN_EVOLVE;
    inputNode->data->hp = inputNode->right->data->hp;
    inputNode->data->id = inputNode->right->data->id;
    inputNode->data->name = inputNode->right->data->name;
    inputNode->data->TYPE = inputNode->right->data->TYPE;
    }

    if(inputNode->data->hp == 0)
    {
        int debug = 2;
        (void)debug;
    }
    inputNode->right = inputNode->right->right;
    return remover(inputNode);
}

void addPokemon(OwnerNode *owner){
    ValidateAll();

    int id = readIntSafe("Enter ID to add: ");
    int check = -1;

    if(findInTree(owner->pokedexRoot, id))
    {
        check = findInTree(owner->pokedexRoot, id)->data->id;
    }

    if(check == id)
    {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }
    creatpokemon(owner, id);
    printf("Pokemon %s (ID %d) added.\n", pokedex[id - 1].name, id);

}

void creatpokemon(OwnerNode *owner, int id)
{
    PokemonNode *newtreenode = allocTreeNode(id);
    addNodeToTree(owner, newtreenode);
}


PokemonNode* allocTreeNode(int id)
{

    PokemonNode* newNode = (PokemonNode*)malloc(sizeof(PokemonNode));
    PokemonData* data = (PokemonData*)malloc(sizeof(PokemonData));
    newNode->data = data;
    if(newNode)
    {
        int idx = id - 1;
        assert(id == pokedex[idx].id);

        newNode->left = newNode->right = NULL;
        newNode->data->id = pokedex[idx].id;
        newNode->data->attack = pokedex[idx].attack;
        newNode->data->CAN_EVOLVE = pokedex[idx].CAN_EVOLVE;
        newNode->data->hp = pokedex[idx].hp;
        newNode->data->name = pokedex[idx].name;
        newNode->data->TYPE = pokedex[idx].TYPE;
    }

    ValidateNode(newNode);

    return newNode;
}

void addNodeToTree(OwnerNode *owner, PokemonNode *newNode)
{
    if(owner->pokedexRoot == NULL)
    {  //Case that the tree is empty

        owner->pokedexRoot = newNode; 
    }
    else
    {     //There is some root node

        PokemonNode* CurrNode = owner->pokedexRoot;
        PokemonNode* NextNode = owner->pokedexRoot;

        while(NextNode != NULL)
        {
            CurrNode = NextNode;
            NextNode = (CurrNode->data->id > newNode->data->id) ? CurrNode->left : CurrNode->right;
        }

        if(CurrNode->data->id > newNode->data->id)
        {
            CurrNode->left = newNode;
        }
        else
        {
            CurrNode->right = newNode;
        }
    }
}


void openPokedexMenu()
{
    
    printf("Your name: ");
    char* name = getDynamicInput();
    if(compareNames(name))
    {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", name);
        return;
    }
    printf("Choose Starter:\n");
    printf("1. Bulbasaur\n");
    printf("2. Charmander\n");
    printf("3. Squirtle\n");
    int choice = readIntSafe("Your choice: ");
    OwnerNode* ourOwnerNode = createOwner(name);
    
    int init_id_lut[] = {0, 1, 4, 7};
    assert(choice >= 1 && choice <= 3);
    int id = init_id_lut[choice];

    creatpokemon(ourOwnerNode, id);
    printf("New Pokedex created for %s with starter %s.\n", ourOwnerNode->ownerName, ourOwnerNode->pokedexRoot->data->name);
    ValidateAll();
}


OwnerNode *createOwner(char *ownerName)
{
    OwnerNode *newownernode = AllocNewOwnerNode(ownerName);
    AddNodeInLast(&ownerHead, newownernode);
    return newownernode;
}


void AddNodeInLast(OwnerNode** listHead, OwnerNode* newNode)
{
    OwnerNode* pLastNode = GetLastNode(*listHead);

    //Case of empty list
    if(pLastNode == NULL)
    {
        AddNodeInHead(listHead, newNode);
    }
    else
    {
        AddNodeInPlace(pLastNode, newNode);
    }

}

//Returns NULL if list is empty. Returns pointer to last item if list is non-empty.
OwnerNode* GetLastNode(OwnerNode* listHead)
{
    return (listHead ? listHead->prev : NULL);
}

int compareNames(char* name)
{
    if(ownerHead == NULL)
    {
        return 0;
    }
    OwnerNode* currNode = ownerHead;

    while(currNode->next && currNode->next != ownerHead)
    {        
        if(strcmp(currNode->ownerName, name) == 0)
        {
            return 1;
        }
        currNode = currNode->next;
    }

    return 0;
}
//This function adds a node after an existing node
void AddNodeInPlace(OwnerNode* lastNode, OwnerNode* newNode)
{
    newNode->next = lastNode->next;
    newNode->prev = lastNode;
    lastNode->next = newNode;
    newNode->next->prev = newNode;
}

void FreeOwnerNode(OwnerNode* node)
{
    if(node)
    {
        if(node->ownerName)
        {
            free(node->ownerName);
        }
        free(node);
    }
    
}

void RemoveOwnerNode(OwnerNode** listHead, OwnerNode* node)
{
    //This is the last node in the list
    if(node->next == node)
    {
        *listHead = NULL;
        return;
    }
    //Case that this is the first node
    if(*listHead == node)
    {
        *listHead = node->next;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;
}

void AddNodeInHead(OwnerNode** listHead, OwnerNode* newNode)
{
    newNode->next = newNode;
    newNode->prev = newNode;
    *listHead = newNode;
}


OwnerNode* AllocNewOwnerNode(char *ownerName)
{
    OwnerNode* newNode = (OwnerNode*)malloc(sizeof(OwnerNode));
    if(newNode)
    {
        newNode->ownerName = ownerName;
    }
    return newNode;
}


// PokemonNode *createPokemonNode(const PokemonData *data){
    // 
// }
// 
// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
        ValidateAll();

    } while (choice != 7);
}

void printOwnersCircular(void)
{
    if(ownerHead == NULL)
    {
        printf("No owners.\n");
        return;
    }

    OwnerNode* currNode = ownerHead;
    printf("Enter direction (F or B): ");
    char* direction = getDynamicInput();
    int printCnt = readIntSafe("How many prints? ");

    for(int i = 0 ; i < printCnt ; i++)
    {
        printf("[%d] %s\n", i+1, currNode->ownerName);
        currNode = (direction[0] == 'F' || direction[0] == 'f') ? currNode->next : currNode->prev;
    }
}

void sortOwners()
{
    if(ownerHead == NULL || ownerHead->next == NULL)
    {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }

    int isSorted;
    do
    {
        isSorted = 1;
        OwnerNode* currNode = ownerHead;
        while(currNode != NULL && currNode->next != ownerHead)
        {
            if(strcmp(currNode->ownerName, currNode->next->ownerName) > 0)
            {
                swapOwnerNodes(&ownerHead, currNode);
                isSorted = 0;
            }
            else
            {
                currNode = currNode->next;
            }
        }
    } while(isSorted == 0);

    printf("Owners sorted by name.\n");
}

void swapOwnerNodes(OwnerNode** listHead, OwnerNode* nodeToSwap)
{
    RemoveOwnerNode(listHead, nodeToSwap);
    AddNodeInPlace(nodeToSwap->next, nodeToSwap);
}

OwnerNode* SearchOwner(OwnerNode* head, char* name)
{
    if(head == NULL)
    {
        return NULL;
    }
    OwnerNode* currNode = head;

    while(strcmp(currNode->ownerName,name) != 0)
    {
        currNode = currNode->next;
        if(currNode == NULL || currNode == head)
        {
            return NULL;
        }
    }
    return currNode;
}

void mergePokedexMenu()
{
    if(ownerHead == NULL || ownerHead->next == NULL)
    {
        printf("Not enough owners to merge.\n");
        return;
    }
    ValidateAll();
    printf("\n");
    printf("=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char* mergerName = getDynamicInput();
    printf("Enter name of second owner: ");
    char* mergedName = getDynamicInput();

    OwnerNode* ownerMerger = SearchOwner(ownerHead, mergerName);
    OwnerNode* ownerToBeFree = SearchOwner(ownerHead, mergedName);

    assert(ownerMerger);
    assert(ownerToBeFree);

    treeMerge(ownerMerger, ownerToBeFree->pokedexRoot);
    printf("Merging %s and %s...\n", ownerMerger->ownerName, ownerToBeFree->ownerName);
    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", ownerToBeFree->ownerName);
    deletePokedexForMerge(ownerToBeFree);

    ValidateAll();
}

void treeMerge(OwnerNode* targetOwner, PokemonNode* node)
{
    if(node == NULL)
    {
        return;
    }
    addtofirsttree(node, targetOwner);
    treeMerge(targetOwner, node->left);
    treeMerge(targetOwner, node->right);
}

void addtofirsttree(PokemonNode* beingadded, OwnerNode* addedto)
{
    PokemonNode *newtreenode = allocTreeNode(beingadded->data->id);
    addNodeToTree(addedto, newtreenode);
}

void deletePokedexForMerge(OwnerNode* ownerToBeFree)
{
    PokemonNode* node = ownerToBeFree->pokedexRoot;
    freeTree(node);
    RemoveOwnerNode(&ownerHead, ownerToBeFree);
    FreeOwnerNode(ownerToBeFree);
}

void deletePokedex()
{
    ValidateAll();
    if(ownerHead == NULL)
    {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    OwnerNode* currNode = ownerHead;
    OwnerNode* nextNode = currNode->next;
    int choice = deleteMenu();
    for (int i = 1; i < choice; i++)
    {
        currNode = nextNode;
        nextNode = nextNode->next;    
    }

    printf("Deleting %s's entire Pokedex...\n", currNode->ownerName);
    RemoveOwnerNode(&ownerHead, currNode);
    FreeOwnerNode(currNode);
    printf("Pokedex deleted.\n");
    ValidateAll();
}

int deleteMenu()
{
    printf("\n");
    printf("=== Delete a Pokedex ===\n");
    printf("1. %s\n", ownerHead->ownerName);
    OwnerNode* currNode = ownerHead;
    OwnerNode* nextNode = currNode->next;
    int index = 1;
    while(nextNode != ownerHead && nextNode != NULL)
    {
        index++;
        printf("%d. %s\n", index, nextNode->ownerName);
        currNode = nextNode;
        nextNode = nextNode->next;
    }
    int choice = readIntSafe("Choose a Pokedex to delete by number: ");
    return choice;

}
void freeTree(PokemonNode* node)
{
    if(node == NULL)
    {
        return;
    }
    //recursion to the left
    freeTree(node->left);
    //recursion to the right
    freeTree(node->right);
    FreeNode(node);
}

void freeAllOwners(void)
{
    while(ownerHead)
    {
        RemoveOwnerNode(&ownerHead, ownerHead);
        FreeOwnerNode(ownerHead);
    }
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
