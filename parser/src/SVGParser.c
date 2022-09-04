/**
 * @file SVGParser.c
 * @author Anthony Vidovic (1130891)
 * @brief File containing modal functions for all modules
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// ~~~~~ Includes ~~~~~ //
#include "SVGParser.h"
#include "SVGHelper.h"

/**
 * @brief Function that creates and popualates given SVG struct pointer
 * 
 * @param fileName file to parse
 * @return SVG* pointer to svg struct
 */
SVG* createSVG(const char* fileName) {
    if (fileName == NULL || fileName[0] == '\0' || strlen(fileName) == 0)
        return NULL;

    FILE* fp = fopen(fileName, "r");

    if (fp == NULL) return NULL;

    // Use libxml2 to help us parse the svg file
    xmlDoc* doc = NULL;
    xmlNode* root_element = NULL;
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);

    SVG* svg = NULL;
    svg = malloc(sizeof(SVG));

    if(svg == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        return NULL;
    }

    svg->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    svg->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    svg->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    svg->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    svg->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    strcpy(svg->namespace, "");
    strcpy(svg->title, "");
    strcpy(svg->description, "");
    
    if(svg == NULL || svg->rectangles == NULL || svg->circles  == NULL || svg->paths == NULL || svg->groups == NULL || svg->otherAttributes == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        return NULL;
    }
    
    // Sort elements into proper svg struct properties
    addElementsToSVG(svg, root_element);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    fclose(fp);
    return svg;
}


// ~~~~~ toString functions ~~~~~ //

/**
 * @brief Returns a char* containing all items in the given SVG struct
 * 
 * @param img 
 * @return char* 
 */
char* SVGToString(const SVG* img) {
    if(img == NULL) return NULL;
    
    char *otherAttributes = toString(img->otherAttributes);
    char *rectangles = toString(img->rectangles);
    char *circles = toString(img->circles);
    char *paths = toString(img->paths);    
    char *groups = toString(img->groups);
    
    int otherAttributesLength = img->otherAttributes->length > 0 ? strlen(otherAttributes) : 0;
    int rectanglesLength = img->rectangles->length > 0 ? strlen(rectangles) : 0;
    int circlesLength = img->circles->length > 0 ? strlen(circles) : 0;
    int pathsLength = img->paths->length > 0 ? strlen(paths) : 0;
    int groupsLength = img->groups->length > 0 ? strlen(groups) : 0;
    int length = strlen(img->namespace) + strlen(img->title) + strlen(img->description) + otherAttributesLength + rectanglesLength + circlesLength + pathsLength + groupsLength;

    char *str = malloc(sizeof(char) * (length + 300));

    strcpy(str, "namespace: ");
    strcat(str, strlen(img->namespace) > 0 ? img->namespace : "none");
    strcat(str, "\n");
    strcat(str, "title: ");
    strcat(str, strlen(img->title) > 0 ? img->title : "none");
    strcat(str, "\n");
    strcat(str, "description: ");
    strcat(str, strlen(img->description) > 0 ? img->description : "none");
    strcat(str, "\n");
    strcat(str, "\nsvg attributes: ");
    strcat(str, img->otherAttributes->length > 0 ? otherAttributes : "none");
    strcat(str, "\n");
    strcat(str, "\nrectangles: \n");
    strcat(str, img->rectangles->length > 0 ? rectangles : "none");
    strcat(str, "\n");
    strcat(str, "\ncircles: \n");
    strcat(str, img->circles->length > 0 ? circles : "none");
    strcat(str, "\n");
    strcat(str, "\npaths: \n");
    strcat(str, img->paths->length > 0 ? paths : "none");
    strcat(str, "\n");
    strcat(str, "\n-------\nGroups: \n");
    strcat(str, img->groups->length > 0 ? groups : "none");
    strcat(str, "\n");

    free(otherAttributes);
    free(rectangles);
    free(circles);
    free(paths);
    free(groups);
    
    return str;
}

/**
 * @brief Returns a char* containing all items an Attribute struct
 * 
 * @param data 
 * @return char* 
 */
char* attributeToString(void* data) {
    if(data == NULL) return NULL;

    Attribute *attr = (Attribute*)data;;

    int length = strlen(attr->name) + strlen(attr->value) + 5;
    char *str = malloc(sizeof(char) * length);
    int r = snprintf(str, length, "%s=%s, ", attr->name, attr->value);
    
    if(r < 0) return NULL;

    return str;
}

/**
 * @brief Returns a char* containing all items an Rectangle struct
 * 
 * @param data 
 * @return char* 
 */
char* rectangleToString(void* data) {
    if(data == NULL) return NULL;
    
    Rectangle *rect = (Rectangle*)data;

    char *otherAttributes = toString(rect->otherAttributes);
    int length = strlen(rect->units) + strlen(otherAttributes) + 100;
    char *str = calloc(sizeof(char), length);

    int len = snprintf(NULL, 0, "%.2f", rect->x);
    char *rectX = malloc(len + 1);
    snprintf(rectX, len + 1, "%.2f", rect->x);

    len = snprintf(NULL, 0, "%.2f", rect->y);
    char *rectY = malloc(len + 1);
    snprintf(rectY, len + 1, "%.2f", rect->y);

    len = snprintf(NULL, 0, "%.2f", rect->width);
    char *rectWidth = malloc(len + 1);
    snprintf(rectWidth, len + 1, "%.2f", rect->width);

    len = snprintf(NULL, 0, "%.2f", rect->height);
    char *rectHeight = malloc(len + 1);
    snprintf(rectHeight, len + 1, "%.2f", rect->height);
    
    strcpy(str, "x: ");
    strcat(str, rectX);
    strcat(str, "\n");
    strcat(str, "y: ");
    strcat(str, rectY);
    strcat(str, "\n");
    strcat(str, "width: ");
    strcat(str, rectWidth);
    strcat(str, "\n");
    strcat(str, "height: ");
    strcat(str, rectHeight);
    strcat(str, "\n");
    strcat(str, "units: ");
    strcat(str, rect->units);
    strcat(str, "\n");
    strcat(str, "Attributes: ");
    strcat(str, rect->otherAttributes->length > 0 ? otherAttributes : "none");
    strcat(str, "\n");

    free(rectX);
    free(rectY);
    free(rectWidth);
    free(rectHeight);
    free(otherAttributes);

    return str;
}

/**
 * @brief Returns a char* containing all items an Circle struct
 * 
 * @param data 
 * @return char* 
 */
char* circleToString(void* data) {
    if(data == NULL) return NULL;

    Circle *c = (Circle*)data;

    char *otherAttributes = toString(c->otherAttributes);
    int length = strlen(otherAttributes) + 500;
    char *str = calloc(sizeof(char), length);

    int len = snprintf(NULL, 0, "%.2f", c->cx);
    char *circleCX = malloc(len + 1);
    snprintf(circleCX, len + 1, "%.2f", c->cx);

    len = snprintf(NULL, 0, "%.2f", c->cy);
    char *circleCY = malloc(len + 1);
    snprintf(circleCY, len + 1, "%.2f", c->cy);

    len = snprintf(NULL, 0, "%.2f", c->r);
    char *circleR = malloc(len + 1);
    snprintf(circleR, len + 1, "%.2f", c->r);


    strcpy(str, "cx: ");
    strcat(str, circleCX);
    strcat(str, "\n");
    strcat(str, "cy: ");
    strcat(str, circleCY);
    strcat(str, "\n");
    strcat(str, "r: ");
    strcat(str, circleR);
    strcat(str, "\n");
    strcat(str, "units: ");
    strcat(str, c->units);
    strcat(str, "\n");
    strcat(str, "Attributes: ");
    strcat(str, c->otherAttributes->length > 0 ? otherAttributes : "none\n");
    strcat(str, "\n");

    free(circleCX);
    free(circleCY);
    free(circleR);
    free(otherAttributes);

    return str;
}

/**
 * @brief Returns a char* containing all items an Path struct
 * 
 * @param data 
 * @return char* 
 */
char* pathToString(void* data) {
    if(data == NULL) return NULL;

    Path *p = (Path*)data;

    char *otherAttributes = toString(p->otherAttributes);
    int length = strlen(p->data) + strlen(otherAttributes) + 500;
    char *str = calloc(sizeof(char), length);

    int r = snprintf(
            str,
            length,
            "data: %s\nattributes: %s\n", 
            p->data, p->otherAttributes->length > 0 ? otherAttributes : "none"
        );
    
    if(r < 0) return NULL;

    free(otherAttributes);

    return str;
}

/**
 * @brief Returns a char* containing all items an Group struct
 * 
 * @param data 
 * @return char* 
 */
char* groupToString(void* data){
    if(data == NULL) return NULL;

    Group *g = (Group*)data;

    char *otherAttributes = toString(g->otherAttributes);
    char *rectangles = toString(g->rectangles);
    char *circles = toString(g->circles);
    char *paths = toString(g->paths);
    char *groups = toString(g->groups);

    int otherAttributesLength = g->otherAttributes->length > 0 ? strlen(otherAttributes) : 0;
    int rectanglesLength = g->rectangles->length > 0 ? strlen(rectangles) : 0;
    int circlesLength = g->circles->length > 0 ? strlen(circles) : 0;
    int pathsLength = g->paths->length > 0 ? strlen(paths) : 0;
    int groupsLength = g->groups->length > 0 ? strlen(groups) : 0;

    int length = otherAttributesLength + rectanglesLength + circlesLength + pathsLength + groupsLength + 100;
    char *str = calloc(sizeof(char), length);

    sprintf(str, "\nGroup start\n\nAttributes: %s\nRectangles: %s\n\nCircles: %s\n\nPaths: %s\n\nGroups: %s\n\nGroup end\n", 
                otherAttributes, 
                rectangles, 
                circles, 
                paths, 
                groups
            );

    free(otherAttributes);
    free(rectangles);
    free(circles);
    free(paths);
    free(groups);

    return str;
}


// ~~~~~ compare functions ~~~~~ //

int compareAttributes(const void* first, const void* second) {
    return -1;
}

int compareGroups(const void* first, const void* second) {
    return -1;
}

int compareRectangles(const void* first, const void* second) {
    return -1;
}

int compareCircles(const void* first, const void* second) {
    return -1;
}

int comparePaths(const void* first, const void* second) {
    return -1;
}


// ~~~~~ delete functions ~~~~~ //

/**
 * @brief Frees all items in the given SVG struct
 * 
 * @param img 
 */
void deleteSVG(SVG* img) {
    if(img == NULL) return;
    freeList(img->otherAttributes);
    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    free(img);
}

/**
 * @brief Frees all items in a single Attribute struct
 * 
 * @param data 
 */
void deleteAttribute(void* data) {
    Attribute *attr;

    if(data == NULL) return;

    attr = (Attribute*)data;
    free(attr->name);
    free(attr);
}

/**
 * @brief Frees all items in a single Group struct
 * 
 * @param data 
 */
void deleteGroup(void* data) {
    Group *g;

    if(data == NULL) return;
    g = (Group*)data;

    freeList(g->otherAttributes);
    freeList(g->rectangles);
    freeList(g->circles);
    freeList(g->paths);
    freeList(g->groups);
    free(g);
}

/**
 * @brief Frees all items in a single Rectangle struct
 * 
 * @param data 
 */
void deleteRectangle(void* data) {
    Rectangle *rect;
    
    if (data == NULL) return;

    rect = (Rectangle*)data;

    freeList(rect->otherAttributes);
    free(rect);
}

/**
 * @brief Frees all items in a single Circle struct
 * 
 * @param data 
 */
void deleteCircle(void* data) {
    Circle *c;
    
    if (data == NULL) return;

    c = (Circle*)data;

    freeList(c->otherAttributes);
    free(c);
}

/**
 * @brief Frees all items in a single Path struct
 * 
 * @param data 
 */
void deletePath(void* data) {   
    Path *path;

    if(data == NULL) return;

    path = (Path*)data;

    freeList(path->otherAttributes);
    free(path);   
}

/**
 * @brief does nothing
 * 
 * @param data 
 */
void dummyDelete(void* data) {}


// ~~~~~ get functions ~~~~~ //

/**
 * @brief Returns a list of all rectangles in SVG struct
 * 
 * @param img 
 * @return List* 
 */
List* getRects(const SVG* img) {
    if(img == NULL) return NULL;

    List *rectangles = initializeList(&rectangleToString, &dummyDelete, &compareRectangles);

    Node* cur = img->rectangles->head;
    while(cur != NULL) {
        Rectangle *rect = (Rectangle*)cur->data;
        insertBack(rectangles, (void*)rect);
        cur = cur->next;
    }

    if(img->groups->length > 0) 
        findRectanglesInGroup(img->groups, rectangles);

    return rectangles;
}

/**
 * @brief Returns a list of all circles in SVG struct
 * 
 * @param img 
 * @return List* 
 */
List* getCircles(const SVG* img) {
    if(img == NULL) return NULL;
    
    List *circles = initializeList(&circleToString, &dummyDelete, &compareCircles);
    Node* cur = img->circles->head;
    
    while(cur != NULL) {
        Circle *circle = (Circle*)cur->data;
        insertBack(circles, (void*)circle);
        cur = cur->next;
    }
    
    if(img->groups->length > 0) 
        findCirclesInGroup(img->groups, circles);
    
    return circles;
}

/**
 * @brief Returns a list of all paths in SVG struct
 * 
 * @param img 
 * @return List* 
 */
List* getPaths(const SVG* img) {
    if(img == NULL) return NULL;

    List *paths = initializeList(&pathToString, &dummyDelete, &comparePaths);
    Node* cur = img->paths->head;

    while(cur != NULL) {
        Path *p = (Path*)cur->data;
        insertBack(paths, (void*)p);
        cur = cur->next;
    }
    if(img->groups->length > 0) 
        findPathsInGroup(img->groups, paths);

    return paths;
}

/**
 * @brief Returns a list of all groups in SVG struct
 * 
 * @param img 
 * @return List* 
 */
List* getGroups(const SVG* img) {
    if(img == NULL) return NULL;

    List *groups = initializeList(&groupToString, &dummyDelete, &compareGroups);

    if(img->groups->length > 0)
        findGroups(img->groups, groups);

    return groups;
}


// ~~~~~ Num functions ~~~~~ //

/**
 * @brief Function that returns the number of all rectangles with the specified area
 * 
 * @param img 
 * @param area 
 * @return int 
 */
int numRectsWithArea(const SVG* img, float area) {
    if(img == NULL || area < 0 || (img->rectangles->length < 1 && img->groups->length < 1)) return 0;

    int found = 0;
    Node *cur = img->rectangles->head;

    while(cur) {
        Rectangle *rect = (Rectangle*)cur->data;
        float rectArea = rect->width * rect->height;
        if(ceil(rectArea) == ceil(area)) found++;
        cur = cur->next;
    }

    if(img->groups->length > 0)
        searchGroupsForRectArea(img->groups, &found, area);

    return found;
}

/**
 * @brief Function that returns the number of all circles with the specified area
 * 
 * @param img 
 * @param area 
 * @return int 
 */
int numCirclesWithArea(const SVG* img, float area) {
    if(img == NULL || area < 0 || (img->circles->length < 1 && img->groups->length < 1)) return 0;

    int found = 0;
    Node *cur = img->circles->head;

    while(cur) {
        Circle *circle = (Circle*)cur->data;
        float circleArea = M_PI * circle->r * circle->r;
        if(ceil(circleArea) == ceil(area)) found++;
        cur = cur->next;
    }

    if(img->groups->length > 0)
        searchGroupsForCircleArea(img->groups, &found, area);

    return found;
}

/**
 * @brief Function that returns the number of all paths with the specified data - i.e. Path.data field
 * 
 * @param img 
 * @param data 
 * @return int 
 */
int numPathsWithdata(const SVG* img, const char* data) {
    if(img == NULL || data == NULL || (img->paths->length < 1 && img->groups->length < 1)) return 0;

    int found = 0;
    Node *cur = img->paths->head;

    while(cur) {
        Path *p = (Path*)cur->data;
        if(strcmp(p->data, data) == 0) found++;
        cur = cur->next;
    }

    if(img->groups->length > 0)
        searchGroupsForPathData(img->groups, &found, data);

    return found;
}

/**
 * @brief Function that returns the number of all groups with the specified length - see A1 Module 2 for details
 * 
 * @param img 
 * @param len 
 * @return int 
 */
int numGroupsWithLen(const SVG* img, int len) {
    if(img == NULL || len < 0 || img->groups->length < 1) return 0;

    int found = 0;
    searchGroupsForGroupLen(img->groups, &found, len);

    return found;
}

/**
 * @brief Function that returns the number of Attributes in all structs of the SVG
 * 
 * @param img 
 * @return int 
 */
int numAttr(const SVG* img) {
    if(img == NULL) return 0;

    int found = 0;

    // Get num attributes from svg->otherAttributes
    found += img->otherAttributes->length;

    // Get num attributes from svg->rectangles
    Node *curRect = img->rectangles->head;
    while(curRect) {
        
        Rectangle *rect = (Rectangle*)curRect->data;
        found += rect->otherAttributes->length;
        curRect = curRect->next;
    }

    // Get num attributes from svg->circles
    Node *curCircle = img->circles->head;
    while(curCircle) {
        Circle *circle = (Circle*)curCircle->data;
        found += circle->otherAttributes->length;
        curCircle = curCircle->next;
    }
    
    // Get num attributes from svg->paths
    Node *curPath = img->paths->head;
    while(curPath) {
        Path *p = (Path*)curPath->data;
        found += p->otherAttributes->length;
        curPath = curPath->next;
    }
    
    // Get num attributes within all groups
    if(img->groups->length > 0)
        searchGroupsForAttributes(img->groups, &found);

    return found;
}


///////////////////////////////////////////////////////////
/* ASSIGNMENT 2 STUFF                                    */
///////////////////////////////////////////////////////////

/**
 * @brief Create a Valid SVG object
 * 
 * @param fileName 
 * @param schemaFile 
 * @return SVG* 
 */
SVG* createValidSVG(const char* fileName, const char* schemaFile) {
    if (fileName == NULL || fileName[0] == '\0' || strlen(fileName) == 0)
        return NULL;
    
    if (schemaFile == NULL || schemaFile[0] == '\0' || strlen(schemaFile) == 0)
        return NULL;
    
    if(!extensionMatches(schemaFile, ".xsd")) return false;
    if(!extensionMatches(fileName, ".svg")) return NULL;
    
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) return NULL;
    
    FILE* schemaFp = fopen(schemaFile, "r");
    if (schemaFp == NULL) return NULL;
    
    // Use libxml2 to help us parse the svg file
    xmlDoc* doc = NULL;
    xmlNode* root_element = NULL;
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        fclose(schemaFp);
        return NULL;
    }

    int ret = validateAgainstXSD(doc, schemaFile);

    if(ret != 0) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        fclose(schemaFp);
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);

    SVG* svg = NULL;
    svg = malloc(sizeof(SVG));

    if(svg == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        fclose(schemaFp);
        return NULL;
    }

    svg->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    svg->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    svg->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    svg->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    svg->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    strcpy(svg->namespace, "");
    strcpy(svg->title, "");
    strcpy(svg->description, "");
    
    if(svg == NULL || svg->rectangles == NULL || svg->circles == NULL || svg->paths == NULL || svg->groups == NULL || svg->otherAttributes == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(fp);
        fclose(schemaFp);
        return NULL;
    }
    
    // Sort elements into proper svg struct properties
    addElementsToSVG(svg, root_element);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    fclose(fp);
    fclose(schemaFp);
    return svg;
}

/**
 * @brief validates struct against schema file
 * 
 * @param img 
 * @param schemaFile 
 * @return true 
 * @return false 
 */
bool validateSVG(const SVG* img, const char* schemaFile) {
    if(img == NULL || schemaFile == NULL || schemaFile[0] == '\0' || strlen(schemaFile) == 0)
        return false;
    
    if(!extensionMatches(schemaFile, ".xsd")) return false;
    
    FILE* schemaFp = fopen(schemaFile, "r");
    if (schemaFp == NULL) return false;
    /* Validation according to header file */
    
    // Validate base svg struct members
    if(img == NULL || strlen(img->namespace) == 0 || img->title == NULL || img->description == NULL || img->rectangles == NULL || img->circles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL) {
        fclose(schemaFp);
        return false;
    }
    
    // Validate lists in svg struct
    if(!isValidAttributes(img->otherAttributes) || !isValidRectangles(img->rectangles) || !isValidCircles(img->circles) || !isValidPaths(img->paths) || !isValidGroups(img->groups)) {
        fclose(schemaFp);
        return false;
    }
    
    xmlDoc* doc = SVGtoDOC(img);

    if(doc == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(schemaFp);   
        return false;
    }
    
    
    int ret = validateAgainstXSD(doc, schemaFile);

    if(ret != 0) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        fclose(schemaFp);   
        return false;
    }
    xmlFreeDoc(doc);
    xmlCleanupParser();
    fclose(schemaFp);   
    return true;
}

/**
 * @brief Saves SVG struct to a file in SVG format
 * 
 * @param img 
 * @param fileName 
 * @return true 
 * @return false 
 */
bool writeSVG(const SVG* img, const char* fileName) {
    if(img == NULL || fileName == NULL || fileName[0] == '\0' || strlen(fileName) == 0)
        return false;

    if(!extensionMatches(fileName, ".svg")) return false;
    xmlDoc* doc = SVGtoDOC(img);
    if(doc == NULL) return false;

    // Save tree to file
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    
    return true;
}

/**
 * @brief function that updates or sets a new attribute
 * 
 * @param img 
 * @param elemType 
 * @param elemIndex 
 * @param newAttribute 
 * @return true 
 * @return false 
 */
bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) {
    if(img == NULL || img->circles == NULL || img->rectangles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL || elemType < 0 || elemType > 4 || newAttribute == NULL) 
        return false;

    if(newAttribute->name == NULL || newAttribute->value == NULL)
        return false;

    bool success = false;
    char *floatStripped = checkForUnits(newAttribute->value);

    switch (elemType)
    {
    case SVG_IMG:
        if(strcmp(newAttribute->name, "xmlns") == 0) {
            snprintf(img->namespace, sizeof(img->namespace), newAttribute->value);
            success = true;
        } else if(strcmp(newAttribute->name, "title") == 0) {
            snprintf(img->title, sizeof(img->title), newAttribute->value);
            success = true;
        } else if(strcmp(newAttribute->name, "desc") == 0) {
            snprintf(img->description, sizeof(img->description), newAttribute->value);
            success = true;
        } else {
            if(!updateAttribute(newAttribute, img->otherAttributes)) {
                insertBack(img->otherAttributes, newAttribute);
                return true;
            } else {
                success = true;
            }
        }

        if(!success) return false;

        deleteAttribute(newAttribute);
        return true;
    case CIRC:
        if(elemIndex < 0 || elemIndex >= img->circles->length)
            return false;

        Circle *circle = getCirleAtPos(img->circles, elemIndex);
        if(circle == NULL) return false;

        if(strcmp(newAttribute->name, "cx") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            circle->cx = strtof(newAttribute->value, NULL);
            success = true;
        } else if(strcmp(newAttribute->name, "cy") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            circle->cy = strtof(newAttribute->value, NULL);
            success = true;
        } else if(strcmp(newAttribute->name, "r") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            circle->r = strtof(newAttribute->value, NULL);
            success = true;
        } else {
            if(!updateAttribute(newAttribute, circle->otherAttributes)) {
                insertBack(circle->otherAttributes, newAttribute);
                return true;
            } else {
                success = true;
            }
        }

        if(!success) return false;

        deleteAttribute(newAttribute);
        return true;
    case RECT:
        if(elemIndex < 0 || elemIndex >= img->rectangles->length)
            return false;

        Rectangle *rect = getRectAtPos(img->rectangles, elemIndex);
        if(rect == NULL) return false;

        if(strcmp(newAttribute->name, "x") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            rect->x = strtof(newAttribute->value, NULL);
            success = true;
        } else if(strcmp(newAttribute->name, "y") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            rect->y = strtof(newAttribute->value, NULL);
            success = true;
        } else if(strcmp(newAttribute->name, "width") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            rect->width = strtof(newAttribute->value, NULL);
            success = true;
        } else if(strcmp(newAttribute->name, "height") == 0) {
            if(strcasecmp(floatStripped, "invalid") == 0) {
                return false;
            }
            rect->height = strtof(newAttribute->value, NULL);
            success = true;
        } else {
            if(!updateAttribute(newAttribute, rect->otherAttributes)) {
                insertBack(rect->otherAttributes, newAttribute);
                return true;
            } else {
                success = true;
            }
        }

        if(!success) return false;

        deleteAttribute(newAttribute);
        return true;
    case PATH:
        if(elemIndex < 0 || elemIndex >= img->paths->length)
            return false;
            
        Path* path = NULL;

        if (strcmp(newAttribute->name, "d") == 0) {
            Node *cur = img->paths->head;
            int count = 0;

            while(cur) {
                if(count == elemIndex) {
                    cur->data = realloc(cur->data, sizeof(Path) + (strlen(newAttribute->value) + 1) * sizeof(char));
                    strcpy(((Path*)cur->data)->data, newAttribute->value);
                    break;
                } 
                cur = cur->next;
                count++;
            }
            success = true;
        } else {
            path = getPathAtPos(img->paths, elemIndex);
            if(!updateAttribute(newAttribute, path->otherAttributes)) {
                insertBack(path->otherAttributes, newAttribute);
                return true;
            } else {
                success = true;
            }
        }

        if(!success) return false;
        
        deleteAttribute(newAttribute);
        return true;
    case GROUP:
        if(elemIndex < 0 || elemIndex >= img->groups->length)
            return false;

        Group *group = getGroupAtPos(img->groups, elemIndex);
        
        if(group == NULL) return false;
    
        if(!updateAttribute(newAttribute, group->otherAttributes)) {
            insertBack(group->otherAttributes, newAttribute);
            return true;
        } else {
            success = true;
        }

        if(!success) return false;
        
        deleteAttribute(newAttribute);
        return true;
    default:
        return false;
    }

    return false;
}

/**
 * @brief adds a new compoennet to svg struct
 * 
 * @param img 
 * @param type 
 * @param newElement 
 */
void addComponent(SVG* img, elementType type, void* newElement) {
    if(img == NULL || newElement == NULL) return;
    
    if(type == CIRC) {
        Circle *circ = (Circle*)newElement;
        insertBack(img->circles, circ);
    } else if(type == RECT) {
        Rectangle *rect = (Rectangle*)newElement;
        insertBack(img->rectangles, rect);
    } else if(type == PATH) {
        Path *path = (Path*)newElement;
        insertBack(img->paths, path);
    }
}

/**
 * @brief converts an attribute to JSON format
 * 
 * @param a 
 * @return char* 
 */
char* attrToJSON(const Attribute *a) {
    if(a == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    } 
    
    char *json = malloc(sizeof(char) * (strlen(a->name) + strlen(a->value) + 23)); 
    sprintf(json, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
    return json;
}

/**
 * @brief converts a circle to JSON format
 * 
 * @param c 
 * @return char* 
 */
char* circleToJSON(const Circle *c) {
    if(c == NULL || c->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    int length = strlen(c->units) + 500;
    char *json = malloc(sizeof(char) * (length + 1));
    snprintf(json, length, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, c->otherAttributes->length, c->units);

    return json;
}

/**
 * @brief converts a rectangle to JSON format
 * 
 * @param r 
 * @return char* 
 */
char* rectToJSON(const Rectangle *r) {
    if(r == NULL || r->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    int length = strlen(r->units) + 1000;
    char *json = malloc(sizeof(char) * (length + 1));
    snprintf(json, length, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, r->otherAttributes->length, r->units);

    return json;
}

/**
 * @brief converts a path to JSON format
 * 
 * @param p 
 * @return char* 
 */
char* pathToJSON(const Path *p) {
    if(p == NULL || p->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    char data[65];
    snprintf(data, sizeof(data), p->data);

    int length = strlen(p->data) + 100;
    char *json = malloc(sizeof(char) * (length + 1));
    snprintf(json, length, "{\"d\":\"%s\",\"numAttr\":%d}", data, p->otherAttributes->length);

    return json;
}

/**
 * @brief converts a group to JSON format
 * 
 * @param g 
 * @return char* 
 */
char* groupToJSON(const Group *g) {
    if(g == NULL || g->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    int num = g->rectangles->length + g->circles->length + g->paths->length + g->groups->length;

    char *json = malloc(sizeof(char) * 250);
    sprintf(json, "{\"children\":%d,\"numAttr\":%d}", num, g->otherAttributes->length);

    return json;
}

/**
 * @brief converts svg struct to JSON
 * 
 * @param img 
 * @return char* 
 */
char* SVGtoJSON(const SVG* img) {
    if(img == NULL || img->circles == NULL || img->rectangles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "{}");
        return empty;
    }

    List *rects = getRects(img);
    List *circles = getCircles(img);
    List *paths = getPaths(img);
    List *groups = getGroups(img);

    char *json = malloc(sizeof(char) * 500);
    sprintf(json, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", rects->length, circles->length, paths->length, groups->length);

    freeList(rects);
    freeList(circles);
    freeList(paths);
    freeList(groups);

    return json;
}

/**
 * @brief converst a list of attributes to JSON
 * 
 * @param list 
 * @return char* 
 */
char* attrListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;
    
    while(cur) {
        Attribute* attr = (Attribute*)cur->data;
        char *attrJson = attrToJSON(attr);
        length += strlen(attrJson) + 2;

        json = realloc(json, sizeof(char) * length);
        strcat(json, attrJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(attrJson);
        cur = cur->next;
    }

    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

/**
 * @brief converst a list of circles to JSON
 * 
 * @param list 
 * @return char* 
 */
char* circListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;
    
    while(cur) {
        Circle* c = (Circle*)cur->data;
        char *cJson = circleToJSON(c);
        length += strlen(cJson) + 2;

        json = realloc(json, sizeof(char) * length);
        strcat(json, cJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(cJson);
        cur = cur->next;
    }

    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

/**
 * @brief converst a list of rectangles to JSON
 * 
 * @param list 
 * @return char* 
 */
char* rectListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;
    
    while(cur) {
        Rectangle* rect = (Rectangle*)cur->data;
        char *rectJson = rectToJSON(rect);
        length += strlen(rectJson) + 2;

        json = realloc(json, sizeof(char) * length);
        strcat(json, rectJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(rectJson);
        cur = cur->next;
    }

    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

/**
 * @brief converst a list of paths to JSON
 * 
 * @param list 
 * @return char* 
 */
char* pathListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;

    while(cur) {
        Path* path = (Path*)cur->data;
        char *pathJson = pathToJSON(path);
        length += strlen(pathJson) + 2;
        json = realloc(json, sizeof(char) * length);
        strcat(json, pathJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(pathJson);
        cur = cur->next;
    }
    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

/**
 * @brief converst a list of groups to JSON
 * 
 * @param list 
 * @return char* 
 */
char* groupListToJSON(const List *list) {
    if(list == NULL || list->length < 1) {
        char *empty = malloc(sizeof(char) * 3);
        strcpy(empty, "[]");
        return empty;
    }

    int length = 2;
    char *json = malloc(sizeof(char) * length);
    strcpy(json, "[");

    Node *cur = list->head;
    
    while(cur) {
        Group* g = (Group*)cur->data;
        char *gJson = groupToJSON(g);
        length += strlen(gJson) + 2;

        json = realloc(json, sizeof(char) * length);
        strcat(json, gJson);
        if(cur->next != NULL) strcat(json, ",");
        
        free(gJson);
        cur = cur->next;
    }

    json = realloc(json, sizeof(char) * length + 1);
    strcat(json, "]");
    
    return json;
}

/**
 * @brief converst JSON format into an svg struct
 * 
 * @param svgString 
 * @return SVG* 
 */
SVG* JSONtoSVG(const char* svgString) {
    if(svgString == NULL) return NULL;

    SVG *svg = malloc(sizeof(SVG));
    svg->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    svg->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    svg->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    svg->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    svg->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    strcpy(svg->namespace, "http://www.w3.org/2000/svg");
    strcpy(svg->title, "");
    strcpy(svg->description, "");

    char *title = extractValue("title", svgString);
    char *desc = extractValue("descr", svgString);

    if(title != NULL) {
        snprintf(svg->title, sizeof(svg->title), title);
        free(title);
    }

    if(desc != NULL) {
        snprintf(svg->description, sizeof(svg->description), desc);
        free(desc);
    }

    return svg;
}

/**
 * @brief converts JSON format into a rectangle struct
 * 
 * @param svgString 
 * @return Rectangle* 
 */
Rectangle* JSONtoRect(const char* svgString) {
    if(svgString == NULL) return NULL;

    Rectangle *rect = malloc(sizeof(Rectangle));
    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect->x = 0;
    rect->y = 0;
    strcpy(rect->units, "");

    char *x = extractIntValue("x", svgString);
    char *y = extractIntValue("y", svgString);
    char *w = extractIntValue("w", svgString);
    char *h = extractIntValue("h", svgString);
    char *units = extractValue("units", svgString);

    if(x != NULL) {
        rect->x = strtof(x, NULL);
    }

    if(y != NULL) {
        rect->y = strtof(y, NULL);
    }
    
    if(w != NULL) {
        rect->width = strtof(w, NULL);
    }

    if(h != NULL) {
        rect->height = strtof(h, NULL);
    }

    if(units != NULL) {
        strcpy(rect->units, units);
    }
    
    return rect;
}  

/**
 * @brief converts JSON format into a circle struct
 * 
 * @param svgString 
 * @return Circle* 
 */
Circle* JSONtoCircle(const char* svgString) {
    if(svgString == NULL) return NULL;

    Circle *circ = malloc(sizeof(Circle));
    circ->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    circ->cx = 0;
    circ->cy = 0;
    strcpy(circ->units, "");

    char *cx = extractIntValue("cx", svgString);
    char *cy = extractIntValue("cy", svgString);
    char *r = extractIntValue("r", svgString);
    char *units = extractValue("units", svgString);

    if(cx != NULL) {
        circ->cx = strtof(cx, NULL);
    }

    if(cy != NULL) {
        circ->cy = strtof(cy, NULL);
    }
    
    if(r != NULL) {
        circ->r = strtof(r, NULL);
    }

    if(units != NULL) {
        strcpy(circ->units, units);
    }
    
    return circ;
}  

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~ Wrapper Funcs for A3 ~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/**
 * @brief validates an svg
 * 
 * @param filename 
 * @param schemaFile 
 * @return true 
 * @return false 
 */
bool validateSVGWrapper(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);
    
    bool isValid = validateSVG(svg, schemaFile);
    deleteSVG(svg);

    if(!isValid) return false;
    return true;
}

/**
 * @brief creates an svg 
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *createSVGWrapper(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }

    char *json = SVGtoJSON(svg);

    if(json == NULL) {
        deleteSVG(svg);
        free(json);
        return NULL;
    }

    deleteSVG(svg);
    return json;
}

/**
 * @brief gets an svgs rects
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getSVGRects(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }
    
    char *svgRects = rectListToJSON(svg->rectangles);
    if(svgRects == NULL) {
        deleteSVG(svg);
        free(svgRects);
        return NULL;
    }

    char *json = malloc(sizeof(char) * (strlen(svgRects)+1));
    if(json == NULL) {
        deleteSVG(svg);
        free(svgRects);
        free(json);
        return NULL;
    }

    strcpy(json, svgRects);
    
    free(svgRects);
    deleteSVG(svg);
    return json;
}

/**
 * @brief gets an svgs circs
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getSVGCircs(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }
    
    char *svgCircs = circListToJSON(svg->circles);
    if(svgCircs == NULL) {
        deleteSVG(svg);
        free(svgCircs);
        return NULL;
    }
    char *json = malloc(sizeof(char) * (strlen(svgCircs)+1));
    if(json == NULL) {
        deleteSVG(svg);
        free(svgCircs);
        free(json);
        return NULL;
    }

    strcpy(json, svgCircs);
    
    free(svgCircs);
    deleteSVG(svg);

    return json;
}

/**
 * @brief gets an svgs paths
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getSVGPaths(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }
    
    char *svgPaths = pathListToJSON(svg->paths);
    if(svgPaths == NULL) {
        deleteSVG(svg);
        free(svgPaths);
        return NULL;
    }

    char *json = malloc(sizeof(char) * (strlen(svgPaths)+1));
    if(json == NULL) {
        deleteSVG(svg);
        free(svgPaths);
        free(json);
        return NULL;
    }

    strcpy(json, svgPaths);
    
    free(svgPaths);
    deleteSVG(svg);

    return json;
}

/**
 * @brief gets an svgs groups
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getSVGGroups(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }
    
    char *svgGroups = groupListToJSON(svg->groups);
    if(svgGroups == NULL) {
        deleteSVG(svg);
        free(svgGroups);
        return NULL;
    }

    char *json = malloc(sizeof(char) * (strlen(svgGroups)+1));
    if(json == NULL) {
        deleteSVG(svg);
        free(svgGroups);
        free(json);
        return NULL;
    }
    strcpy(json, svgGroups);
    
    free(svgGroups);
    deleteSVG(svg);

    return json;
}

/**
 * @brief gets an svgs title and desc
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getSVGTitleAndDesc(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }
    
    int len = strlen(svg->title) + strlen(svg->description) + 3;
    char *json = malloc(sizeof(char) * len);
    strcpy(json, svg->title);
    strcat(json, ":");
    strcat(json, svg->description);
    
    deleteSVG(svg);
    return json;
}

/**
 * @brief Get the Rect Other Attributes 
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getRectOtherAttributes(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }

    int len = 2;
    char *json = malloc(sizeof(char) * len);
    strcpy(json, "");

    Node* cur = svg->rectangles->head;
    while(cur) {
        Rectangle *rect = (Rectangle*)cur->data;
        char *tempStr = attrListToJSON(rect->otherAttributes);
        len += strlen(tempStr) + 10;
        json = realloc(json, sizeof(char) + len);
        strcat(json, tempStr);
        strcat(json, "|");
        free(tempStr);
        cur = cur->next;
    }

    deleteSVG(svg);
    return json;
}

/**
 * @brief Get the Circ Other Attributes 
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getCircOtherAttributes(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }

    int len = 2;
    char *json = malloc(sizeof(char) * len);
    strcpy(json, "");

    Node* cur = svg->circles->head;
    while(cur) {
        Circle *shape = (Circle*)cur->data;
        char *tempStr = attrListToJSON(shape->otherAttributes);
        len += strlen(tempStr) + 10;
        json = realloc(json, sizeof(char) + len);
        strcat(json, tempStr);
        strcat(json, "|");
        free(tempStr);
        cur = cur->next;
    }

    deleteSVG(svg);
    return json;
}

/**
 * @brief Get the Path Other Attributes 
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getPathOtherAttributes(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }

    int len = 2;
    char *json = malloc(sizeof(char) * len);
    strcpy(json, "");

    Node* cur = svg->paths->head;
    while(cur) {
        Path *shape = (Path*)cur->data;
        char *tempStr = attrListToJSON(shape->otherAttributes);
        len += strlen(tempStr) + 10;
        json = realloc(json, sizeof(char) + len);
        strcat(json, tempStr);
        strcat(json, "|");
        free(tempStr);
        cur = cur->next;
    }

    deleteSVG(svg);
    return json;
}

/**
 * @brief Get the Group Other Attributes
 * 
 * @param filename 
 * @param schemaFile 
 * @return char* 
 */
char *getGroupOtherAttributes(char *filename, char *schemaFile) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return NULL;
    }

    int len = 2;
    char *json = malloc(sizeof(char) * len);
    strcpy(json, "");

    Node* cur = svg->groups->head;
    while(cur) {
        Group *shape = (Group*)cur->data;
        char *tempStr = attrListToJSON(shape->otherAttributes);
        len += strlen(tempStr) + 10;
        json = realloc(json, sizeof(char) + len);
        strcat(json, tempStr);
        strcat(json, "|");
        free(tempStr);
        cur = cur->next;
    }

    deleteSVG(svg);
    return json;
}

/**
 * @brief Set or update attribute of a shape 
 * 
 * @param filename 
 * @param schemaFile 
 * @param name 
 * @param value 
 * @param index 
 * @param elementType 
 * @return true 
 * @return false 
 */
bool setAttributeWrapper(char *filename, char *schemaFile, char *name, char *value, int index, int elementType) {
    SVG *svg = createValidSVG(filename, schemaFile);
    
    bool isValid = validateSVG(svg, schemaFile);
    if(!isValid) {
        deleteSVG(svg);
        return false;
    }
    
    Attribute *attr = malloc(sizeof(Attribute) + (strlen(value) + 1) * sizeof(char));
    attr->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(attr->name, name);
    strcpy(attr->value, value); 

    bool isSet = setAttribute(svg, elementType, index, attr);
    if(!isSet) {
        deleteAttribute(attr);
        deleteSVG(svg);
        return false;
    }

    isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    bool written = writeSVG(svg, filename);   
    if(!written) {
        deleteSVG(svg);
        return false;
    }

    isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    deleteSVG(svg);
    return true;
}

/**
 * @brief adds a component to svg
 * 
 * @param filename 
 * @param schemaFile 
 * @param elementType 
 * @param json 
 * @return true 
 * @return false 
 */
bool addComponentWrapper(char *filename ,char *schemaFile, int elementType, char *json) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);
    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    if(elementType == RECT) {
        Rectangle *rect = JSONtoRect(json);
        addComponent(svg, RECT, rect);
    } else if(elementType == CIRC) {
        Circle *circ = JSONtoCircle(json);
        addComponent(svg, CIRC, circ);
    }

    bool written = writeSVG(svg, filename);   
    if(!written) {
        deleteSVG(svg);
        return false;
    }

    isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return false;
    }
    
    deleteSVG(svg);
    return true;
}

/**
 * @brief scales all rects or circs in an svg by scaleval
 * 
 * @param filename 
 * @param schemaFile 
 * @param elementType 
 * @param scaleVal 
 * @return true 
 * @return false 
 */
bool scaleShape(char *filename, char *schemaFile, int elementType, int scaleVal) {
    SVG *svg = createValidSVG(filename, schemaFile);

    bool isValid = validateSVG(svg, schemaFile);
    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    if(elementType == RECT) {
        Node *cur = svg->rectangles->head;
        while(cur) {
            Rectangle *rect = (Rectangle*)cur->data;
            rect->width *= scaleVal; 
            rect->height *= scaleVal;
            cur = cur->next;
        }
        scaleRectsInGroups(scaleVal, svg->groups);
    } else if(elementType == CIRC) {
        Node *cur = svg->circles->head;
        while(cur) {
            Circle *c = (Circle*)cur->data;
            c->r *= scaleVal; 
            cur = cur->next;
        }
        scaleCircsInGroups(scaleVal, svg->groups);
    }

    bool written = writeSVG(svg, filename);   
    if(!written) {
        deleteSVG(svg);
        return false;
    }

    isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    return true;
}

/**
 * @brief scales all rects in all groups
 * 
 * @param scaleVal 
 * @param groups 
 */
void scaleRectsInGroups(int scaleVal, List *groups) {
    Node *cur = groups->head;

    while (cur) {
        Group *g = (Group*)cur->data;

        if(g->rectangles->length > 0) {
            Node *curRect = g->rectangles->head;

            // Loop until no rectangles in list
            while(curRect) {
                Rectangle *rect = (Rectangle*)curRect->data;
                rect->width *= scaleVal; 
                rect->height *= scaleVal;
                curRect = curRect->next;
            }
        }
        scaleRectsInGroups(scaleVal, g->groups);
        cur = cur->next;
    }
}

/**
 * @brief scales all circles in groups
 * 
 * @param scaleVal 
 * @param groups 
 */
void scaleCircsInGroups(int scaleVal, List *groups) {
    Node *cur = groups->head;

    while (cur) {
        Group *g = (Group*)cur->data;

        if(g->circles->length > 0) {
            Node *curCirc = g->circles->head;

            // Loop until no circles in list
            while(curCirc) {
                Circle *circ = (Circle*)curCirc->data;
                circ->r *= scaleVal; 
                curCirc = curCirc->next;
            }
        }
        scaleCircsInGroups(scaleVal, g->groups);
        cur = cur->next;
    }
}

/**
 * @brief Create a New S V G object
 * 
 * @param filename 
 * @param schemaFile 
 * @param json 
 * @return true 
 * @return false 
 */
bool createNewSVG(char *filename, char *schemaFile, char *json) {
    SVG *svg = JSONtoSVG(json);

    bool isValid = validateSVG(svg, schemaFile);
    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    bool written = writeSVG(svg, filename);   
    if(!written) {
        deleteSVG(svg);
        return false;
    }

    isValid = validateSVG(svg, schemaFile);

    if(!isValid) {
        deleteSVG(svg);
        return false;
    }

    return true;
}