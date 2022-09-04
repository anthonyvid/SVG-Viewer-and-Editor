/**
 * @file SVGHelper.c
 * @author Anthony Vidovic (1130891)
 * @brief Helper functions for svg parser library
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

// ~~~~~ Includes ~~~~~ //
#include "SVGHelper.h"

/**
 * @brief iterates xml tree starting from the root node,
 * will sort/add elements and attributes into svg struct
 * 
 * @param svg pointer to SVG struct
 * @param a_node 
 */
void addElementsToSVG(SVG* svg, xmlNode* a_node) {
    xmlNode* cur_node = NULL;

    // Keep looping to the next node until there is none 
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            xmlAttr* attribute = cur_node->properties;
            char *name = (char*)cur_node->name;
            
            if (strcasecmp(name, "SVG") == 0) {
                char* namespace = malloc(sizeof(char) * (xmlStrlen(cur_node->ns->href) + 1));

                if((char*)cur_node->ns->href != NULL) 
                    strcpy(namespace, (char*)cur_node->ns->href);

                snprintf(svg->namespace, sizeof(svg->namespace), namespace);

                while(attribute) { 
                    if(attribute == NULL) continue;
                    insertBack(svg->otherAttributes, newAttribute(attribute));
                    attribute = attribute->next;
                }    
                free(namespace);
            }
            else if (strcasecmp(name, "TITLE") == 0) {
                snprintf(svg->title, sizeof(svg->title), (char*)cur_node->children->content);
            }
            else if (strcasecmp(name, "DESC") == 0) {
                snprintf(svg->description, sizeof(svg->description), (char*)cur_node->children->content);
            }
            else if (strcasecmp(name, "G") == 0) {
                insertBack(svg->groups, createGroup(cur_node, attribute));
                continue;
            }
            else if (strcasecmp(name, "PATH") == 0) {
                insertBack(svg->paths, createPath(cur_node, attribute));
            }
            else if (strcasecmp(name, "CIRCLE") == 0) {
                insertBack(svg->circles, createCircle(cur_node, attribute));
            }
            else if (strcasecmp(name, "RECT") == 0) {
                insertBack(svg->rectangles, createRectangle(cur_node, attribute));
            }
        }
        // Recursive call, passing the first nested node from the parent node
        addElementsToSVG(svg, cur_node->children);
    }
}

/**
 * @brief Create a Rectangle object
 * 
 * @param cur_node Rectangle node/element
 * @param attribute first attribute of Rectangle node
 * @return Rectangle* 
 */
Rectangle* createRectangle(xmlNode* cur_node, xmlAttr* attribute) {
    Rectangle *rect = malloc(sizeof(Rectangle));
    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect->x = 0;
    rect->y = 0;
    strcpy(rect->units, "");

    while(attribute) { 
        if(attribute == NULL) continue;
        char *attrName = (char*)attribute->name;
        char *attrValue = (char*)attribute->children->content;
        
        if(strcasecmp(attrName, "x") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(rect->units, units);
            rect->x = strtof(attrValue, NULL);
            free(num);
        } else if(strcasecmp(attrName, "y") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(rect->units, units);
            rect->y = strtof(num, NULL);
            free(num);
        } else if(strcasecmp(attrName, "width") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(rect->units, units);
            rect->width = strtof(num, NULL);
            free(num);
        } else if(strcasecmp(attrName, "height") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(rect->units, units);
            rect->height = strtof(num, NULL);
            free(num);
        } else {
            insertBack(rect->otherAttributes, newAttribute(attribute));
        }
        attribute = attribute->next;
    }
    return rect;
}

/**
 * @brief Create a Circle object
 * 
 * @param cur_node Circle node/element
 * @param attribute first attribute of Circle node
 * @return Circle* 
 */
Circle* createCircle(xmlNode* cur_node, xmlAttr* attribute) {
    Circle *circle = malloc(sizeof(Circle));
    circle->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    circle->cx = 0;
    circle->cy = 0;
    strcpy(circle->units, "");
    
    while(attribute) { 
        if(attribute == NULL) continue;
        char *attrName = (char*)attribute->name;
        char *attrValue = (char*)attribute->children->content;
        
        if(strcasecmp(attrName, "cx") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(circle->units, units);     
            circle->cx = strtof(num, NULL);
            free(num);
        } else if(strcasecmp(attrName, "cy") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(circle->units, units);      
            circle->cy = strtof(num, NULL);
            free(num);
        } else if(strcasecmp(attrName, "r") == 0) {
            char *units = checkForUnits(attrValue);
            char *num = removeAllExceptDigits(attrValue);
            if(strlen(units) > 0) strcpy(circle->units, units);     
            circle->r = strtof(num, NULL);
            free(num);
        } else {
            insertBack(circle->otherAttributes, newAttribute(attribute));
        }
        attribute = attribute->next;        
    }
    return circle;
}

/**
 * @brief Create a Path object
 * 
 * @param cur_node Path node/element
 * @param attribute first attribute of Path node
 * @return Path* 
 */
Path* createPath(xmlNode* cur_node, xmlAttr* attribute) {
    Path *p = malloc(sizeof(Path) + (strlen((char*)attribute->children->content) + 1) * sizeof(char));
    p->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    
    while(attribute) { 
        if(attribute == NULL) continue;
        char *attrName = (char*)attribute->name;
        char *attrValue = (char*)attribute->children->content;
        
        if(strcasecmp(attrName, "d") == 0) {
            p = realloc(p, sizeof(Path) + (strlen((char*)attribute->children->content) + 1) * sizeof(char));
            strcpy(p->data, attrValue);
        } else {
            insertBack(p->otherAttributes, newAttribute(attribute));
        }
        attribute = attribute->next;
    }
    return p;
}

/**
 * @brief Create a Group object
 * 
 * @param cur_node group node/element
 * @param attribute first attribute of group node
 * @return Group* 
 */
Group* createGroup(xmlNode* cur_node, xmlAttr* attribute) {
    Group *g = malloc(sizeof(Group));
    g->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    g->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    g->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    g->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    g->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    // Add attributes of group tag
    while(attribute) { 
        if(attribute == NULL) continue;
        insertBack(g->otherAttributes, newAttribute(attribute));
        attribute = attribute->next;
    }   

    iterateGroup(cur_node->children, g);
    return g;
}

/**
 * @brief recursively iterates a groups items
 * 
 * @param a_node first item in the group
 * @param g pointer to a Group struct
 */
void iterateGroup(xmlNode* a_node, Group *g) {
    xmlNode* cur_node = NULL;
    
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {  
            xmlAttr* tempAttribute = cur_node->properties;
            char *name = (char*)cur_node->name;

            if (strcasecmp(name, "G") == 0) {
                insertBack(g->groups, createGroup(cur_node, tempAttribute));
                continue;
            } else if (strcasecmp(name, "RECT") == 0) {
                insertBack(g->rectangles, createRectangle(cur_node, tempAttribute));
            } else if (strcasecmp(name, "CIRCLE") == 0) {
                insertBack(g->circles, createCircle(cur_node, tempAttribute));
            } else if (strcasecmp(name, "PATH") == 0) {
                insertBack(g->paths, createPath(cur_node, tempAttribute));
            }
        }    
        iterateGroup(cur_node->children, g);       
    }
}

/**
 * @brief creates instance of Attribute struct and sets its name and value
 * 
 * @param xmlAttr 
 * @return Attribute* populated attribute struct 
 */
Attribute *newAttribute(xmlAttr* xmlAttr) {
    char *attrName = (char*)xmlAttr->name;
    char *attrValue = (char*)xmlAttr->children->content;
    
    Attribute *attr = malloc(sizeof(Attribute) + (strlen(attrValue) + 1) * sizeof(char));
    attr->name = malloc(sizeof(char) * (strlen(attrName) + 1));

    strcpy(attr->name, attrName);
    strcpy(attr->value, attrValue); 

    return attr;    
}

/**
 * @brief removes all characters from a string except for digits,
 * modified to keep '-' for negative numbers and '.' for decimals.
 * 
 * https://www.codevscolor.com/c-remove-characters-string-keeping-numbers
 * @param string dirty string (any characters)
 * @return char* string containing only digits, or empty string if no digits present
 */
char *removeAllExceptDigits(char *string) {
    if(strlen(string) < 1) return NULL;

    char *digitsStr = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(digitsStr, string); 
    bool negative = false;
    int j = 0;
    
    // Flags negative if '-' is present in string (added by me)
    for (int i = 0; digitsStr[i] != '\0'; i++) {
        if(digitsStr[i] == '-' && (digitsStr[i+1] >= '0' && digitsStr[i+1] <= '9')) {
            negative = true;
        }
    }
    
    // Removes everything except digits
    for (int i = 0; digitsStr[i]; i++) {
        if ((digitsStr[i] >= '0' && digitsStr[i] <= '9') || digitsStr[i] == '.') {
            digitsStr[j] = digitsStr[i];
            j++;
        }
    }

    digitsStr[j] = '\0';

    // Prepend minus sign if negative flaged
    if(negative) digitsStr = prependChar('-', digitsStr);

    // Remove trailing decimal points if there is any
    if(strlen(digitsStr) > 0)
        if(digitsStr[strlen(digitsStr)-1] == '.') digitsStr[strlen(digitsStr)-1] = '\0';

    return digitsStr;
}

/**
 * @brief prepends a character to a string
 * 
 * https://stackoverflow.com/questions/50052254/add-char-to-beginning-and-end-of-a-string
 * @param before char to prepend
 * @param str 
 * @return char* string with prepended character
 */
char *prependChar(char before, char *str) {
    size_t len = strlen(str);
    if(before) {
        memmove(str + 1, str, ++len);
        *str = before;
    }
    return str;
}

/**
 * @brief removes all spaces from a string
 * 
 * https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
 * @param s dirty string with possible spaces
 */
void remove_spaces(char* s) {
    if(s == NULL) return;

    char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while ((*s++ = *d++));
}

/**
 * @brief checks if a string contains a supported length unit identifier
 * 
 * @param string 
 * @return char* the unit that exists, or NULL if none found. Valid if digit comes before the unit.
 */
char *checkForUnits(char *string) {
    const int NUM_UNITS = 9;
    char *tempStr = malloc(sizeof(char) * (strlen(string) + 1));

    /* The supported length unit identifiers in SVG are: 
    /  emx, px, pt, pc, cm, mm, in, and percentages.
    */
    char *units[] = {"cm", "px", "em", "ex", "pt", "pc", "mm", "in", "%"};

    strcpy(tempStr, string);
    
    for(int i = 0; i < strlen(tempStr); i++)
        tempStr[i] = tolower(tempStr[i]);
    
    remove_spaces(tempStr);
    
    // Remove number from string
    char *tempStr2 = removeDigitsFromStr(tempStr);

    remove_spaces(tempStr2);

    if(strlen(tempStr2) < 1) {
        free(tempStr);
        free(tempStr2);
        return "";
    }

    for(int i = 0; i < NUM_UNITS; i++) {
        if(strcmp(tempStr2, units[i]) == 0) {
            char *ret = strstr(tempStr, units[i]);
            int pos = ret - tempStr;

            if(isdigit(tempStr[pos-1])) {
                free(tempStr);
                free(tempStr2);
                return units[i];
            }
        }
    }
    
    free(tempStr);
    free(tempStr2);
    return "invalid";
}

/**
 * @brief removes all digits from a string, modified to keep decimals also
 * 
 * https://stackoverflow.com/questions/17557477/strip-numbers-from-a-string-in-c
 * @param stringToStrip dirty string with possible digits
 */
char* removeDigitsFromStr(char *stringToStrip) {
    char *stripped = malloc(sizeof(char) * (strlen(stringToStrip) + 1));
    
    const int stringLen = strlen(stringToStrip);
    int j = 0;
    char currentChar;
    
    for( int i = 0; i < stringLen; ++i ) {
        currentChar = stringToStrip[i];
        if (currentChar != '.' && !isdigit(currentChar)) {
            stripped[j++] = currentChar;
        }
    }
    
    stripped[j] = '\0';

    return stripped;
}

/**
 * @brief recursively iterates a group and adds all rectangles to rectangles list
 * 
 * @param group group to iterate
 * @param rectangles list to add to
 */
void findRectanglesInGroup(List *group, List *rectangles) {
    if(group == NULL || rectangles == NULL) return;
    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        
        if(g->rectangles->length > 0) {
            Node *curRect = g->rectangles->head;
            while(curRect) {
                Rectangle *r = (Rectangle*)curRect->data;
                insertBack(rectangles, r);
                curRect = curRect->next;
            }
        }
        findRectanglesInGroup(g->groups, rectangles);
        cur = cur->next;
    }
}

/**
 * @brief recursively iterates a group and adds all circles to circles list
 * 
 * @param group group to iterate
 * @param circles list to add to
 */
void findCirclesInGroup(List *group, List *circles) {
    if(group == NULL || circles == NULL) return;
    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        
        if(g->circles->length > 0) {
            Node *curCircle = g->circles->head;
            while(curCircle) {
                Circle *circle = (Circle*)curCircle->data;
                insertBack(circles, circle);
                curCircle = curCircle->next;
            }
        }
        findCirclesInGroup(g->groups, circles);
        cur = cur->next;
    }
}

/**
 * @brief recursively iterates a group and adds all paths to paths list
 * 
 * @param group group to iterate
 * @param paths list to add to
 */
void findPathsInGroup(List *group, List *paths) {
    if(group == NULL || paths == NULL) return;
    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        
        if(g->paths->length > 0) {
            Node *curPath = g->paths->head;
            while(curPath) {
                Circle *p = (Circle*)curPath->data;
                insertBack(paths, p);
                curPath = curPath->next;
            }
        }
        findPathsInGroup(g->groups, paths);
        cur = cur->next;
    }
}

/**
 * @brief recursively iterates a group and adds all groups to groups list
 * 
 * @param group group to iterate
 * @param groups list to add to
 */
void findGroups(List *group, List *groups) {
    if(group == NULL || groups == NULL) return;
    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        insertBack(groups, g);

        if(g->groups->length > 0) {
            findGroups(g->groups, groups);
        }
        cur = cur->next;
    }
}

/**
 * @brief Recursively iterates groups and checks if any rectangle found has
 * the given area
 * 
 * @param group group to recusively search
 * @param found number of matches found
 * @param area area to check for
 */
void searchGroupsForRectArea(List *group, int *found, float area) {
    if(group == NULL || area < 0) return;

    Node *cur = group->head;

    while (cur) {
        Group *g = (Group*)cur->data;

        if(g->rectangles->length > 0) {
            Node *curRect = g->rectangles->head;

            // Loop until no rectangles in list
            while(curRect) {
                Rectangle *rect = (Rectangle*)curRect->data;
                float rectArea = rect->width * rect->height;
                if(ceil(rectArea) == ceil(area)) *found+=1;
                curRect = curRect->next;
            }
        }
        // Recursive call to search groups inside of the current group
        searchGroupsForRectArea(g->groups, found, area);
        cur = cur->next;
    }
}

/**
 * @brief Recursively iterates groups and checks if any circle found has
 * the given area
 * 
 * @param group group to recusively search
 * @param found number of matches found
 * @param area area to check for
 */
void searchGroupsForCircleArea(List *group, int *found, float area) {
    if(group == NULL || area < 0) return;

    Node *cur = group->head;

    while (cur) {
        Group *g = (Group*)cur->data;
        if(g->circles->length > 0) {
            Node *curCircle = g->circles->head;

            // Loop until no circles in list
            while(curCircle) {
                Circle *circle = (Circle*)curCircle->data;
                float circleArea = M_PI * circle->r * circle->r;
                if(ceil(circleArea) == ceil(area)) *found+=1;
                curCircle = curCircle->next;
            }
        }
        // Recursive call to search groups inside of the current group
        searchGroupsForCircleArea(g->groups, found, area);
        cur = cur->next;
    }
}

/**
 * @brief Recursively iterates groups and checks if any path found has
 * the given data
 * 
 * @param group group to recusively search
 * @param found number of matches found
 * @param data data to check for
 */
void searchGroupsForPathData(List *group, int *found, const char *data) {
    if(group == NULL || data == NULL) return;

    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        if(g->paths->length > 0) {
            Node *curPath = g->paths->head;

            // Loop until no paths in list
            while(curPath) {
                Path *p = (Path*)curPath->data;
                if(strcmp(p->data, data) == 0) *found+=1;
                curPath = curPath->next;
            }
        }
        // Recursive call to search groups inside of the current group
        searchGroupsForPathData(g->groups, found, data);
        cur = cur->next;
    }
}

/**
 * @brief Recursively iterates groups and checks if any group found has
 * the given length
 * 
 * @param group group to recusively search
 * @param found number of matches found
 * @param len length to check for
 */
void searchGroupsForGroupLen(List *group, int *found, int len) {
    if(group == NULL) return;

    Node *cur = group->head;
    
    // Loop until no groups in list
    while(cur) {
        Group *g = (Group*)cur->data;
        int groupLen = g->rectangles->length + g->circles->length + g->paths->length + g->groups->length;
        if(groupLen == len) *found+=1;

        // Recursive call to search groups inside of the current group
        searchGroupsForGroupLen(g->groups, found, len);
        cur = cur->next;
    }
}

/**
 * @brief Recrusive function that finds the number of other attributes in groups
 * 
 * @param group group to recursively search
 * @param found number of matches found
 */
void searchGroupsForAttributes(List *group, int *found) {
    if(group == NULL) return;

    Node *cur = group->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        
        // Get otherAttributes from groups
        *found += g->otherAttributes->length;

        // Get num attributes from g->rectangles
        Node *curRect = g->rectangles->head;
        while(curRect) {
            Rectangle *rect = (Rectangle*)curRect->data;
            *found += rect->otherAttributes->length;
            curRect = curRect->next;
        }

        // Get num attributes from g->circles
        Node *curCircle = g->circles->head;
        while(curCircle) {
            Circle *circle = (Circle*)curCircle->data;
            *found += circle->otherAttributes->length;
            curCircle = curCircle->next;
        }
        
        // Get num attributes from g->paths
        Node *curPath = g->paths->head;
        while(curPath) {
            Path *p = (Path*)curPath->data;
            *found += p->otherAttributes->length;
            curPath = curPath->next;
        }

        if(g->groups->length > 0)
            // Recursive call to search groups inside of the current group
            searchGroupsForAttributes(g->groups, found);
        cur = cur->next;
    }
}

/**
 * @brief checks if a file has a specific extension
 * 
 * @param fileName 
 * @param extension 
 * @return true 
 * @return false 
 */
bool extensionMatches(const char *fileName, char *extension) {
    char *ext = strrchr(fileName, '.');

    if (!ext || strcasecmp(ext, extension) != 0) return false;
    return true;
}

/**
 * @brief converts an svg struct to an xmlDoc
 * 
 * @param svg 
 * @return xmlDoc* 
 */
xmlDoc* SVGtoDOC(const SVG *svg) {
    if(svg == NULL) return NULL;
    
    xmlDoc* doc = NULL;
    xmlNodePtr root_node = NULL;

    doc = xmlNewDoc(BAD_CAST "1.0");

    root_node = xmlNewNode(NULL, BAD_CAST "svg");

    // Create namespace
    xmlNsPtr nsPtr = xmlNewNs(root_node, (xmlChar*) svg->namespace, NULL);

    if(nsPtr == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    xmlSetNs(root_node, nsPtr);
    xmlDocSetRootElement(doc, root_node);
    
    // Add Title if its present
    if(strlen(svg->title) > 0)
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST svg->title);

    // Add description if its present
    if(strlen(svg->description) > 0)
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST svg->description);

    // Add other attributes for svg node
    if(svg->otherAttributes->length > 0)
        addOtherAttributesToNode(root_node, svg->otherAttributes);

    // Add rectangles to tree
    if(svg->rectangles->length > 0)
        addRectanglesToParent(root_node, svg->rectangles);
    
    // Add circles to tree
    if(svg->circles->length > 0)
        addCirclesToParent(root_node, svg->circles);

    // Add paths to tree
    if(svg->paths->length > 0)
        addPathsToParent(root_node, svg->paths);

    // Add groups to tree
    if(svg->groups->length > 0)
        addGroupsToParent(root_node, svg->groups);
    
    return doc;
}

/**
 * @brief adds all rectangles in list to the node given
 * 
 * @param node 
 * @param rectangles 
 */
void addRectanglesToParent(xmlNodePtr node, List *rectangles) {
    if(node == NULL || rectangles == NULL) return;
    
    Node *cur = rectangles->head;
    while(cur) {
        Rectangle* rect = (Rectangle*)cur->data;
        xmlNodePtr rectNode = xmlNewChild(node, NULL, BAD_CAST "rect", NULL);

        char x[500], y[500], width[500], height[500];
        char *units = malloc(sizeof(char) * (strlen(rect->units) + 1));
        strcpy(units, rect->units);

        sprintf(x, "%f%s", rect->x, units);
        sprintf(y, "%f%s", rect->y, units);
        sprintf(width, "%f%s", rect->width, units);
        sprintf(height, "%f%s", rect->height, units);

        xmlNewProp(rectNode, BAD_CAST "x", BAD_CAST x);
        xmlNewProp(rectNode, BAD_CAST "y", BAD_CAST y);
        xmlNewProp(rectNode, BAD_CAST "width", BAD_CAST width);
        xmlNewProp(rectNode, BAD_CAST "height", BAD_CAST height);

        addOtherAttributesToNode(rectNode, rect->otherAttributes);
        free(units);
        cur = cur->next;
    }
}

/**
 * @brief adds all circles in list to the node given
 * 
 * @param node 
 * @param rectangles 
 */
void addCirclesToParent(xmlNodePtr node, List *circles) {
    if(node == NULL || circles == NULL) return;
    
    Node *cur = circles->head;
    while(cur) {
        Circle* circle = (Circle*)cur->data;
        xmlNodePtr circleNode = xmlNewChild(node, NULL, BAD_CAST "circle", NULL);

        char cx[500], cy[500], radius[500];
        char *units = malloc(sizeof(char) * (strlen(circle->units) + 1));
        strcpy(units, circle->units);

        sprintf(cx, "%f%s", circle->cx, units);
        sprintf(cy, "%f%s", circle->cy, units);
        sprintf(radius, "%f%s", circle->r, units);

        xmlNewProp(circleNode, BAD_CAST "cx", BAD_CAST cx);
        xmlNewProp(circleNode, BAD_CAST "cy", BAD_CAST cy);
        xmlNewProp(circleNode, BAD_CAST "r", BAD_CAST radius);

        addOtherAttributesToNode(circleNode, circle->otherAttributes);
        free(units);
        cur = cur->next;
    }
}

/**
 * @brief adds all paths in list to the node given
 * 
 * @param node 
 * @param rectangles 
 */
void addPathsToParent(xmlNodePtr node, List *paths) {
    if(node == NULL || paths == NULL) return;

    Node *cur = paths->head;
    while(cur) {
        Path* p = (Path*)cur->data;
        xmlNodePtr pNode = xmlNewChild(node, NULL, BAD_CAST "path", NULL);

        xmlNewProp(pNode, BAD_CAST "d", BAD_CAST p->data);
        addOtherAttributesToNode(pNode, p->otherAttributes);
        cur = cur->next;
    }
}

/**
 * @brief adds all groups and groups children in list to the node given
 * 
 * @param node 
 * @param rectangles 
 */
void addGroupsToParent(xmlNodePtr node, List *groups) {
    if(node == NULL || groups == NULL) return;

    Node *cur = groups->head;

    while(cur) {
        Group *g = (Group*)cur->data;
        xmlNodePtr gNode = xmlNewChild(node, NULL, BAD_CAST "g", NULL);

        addOtherAttributesToNode(gNode, g->otherAttributes);

        if(g->rectangles->length > 0)
            addRectanglesToParent(gNode, g->rectangles);

        if(g->circles->length > 0)
            addCirclesToParent(gNode, g->circles);

        if(g->paths->length > 0)
            addPathsToParent(gNode, g->paths);

        if(g->groups->length > 0)
            addGroupsToParent(gNode, g->groups);

        cur = cur->next;
    }
}

/**
 * @brief adds the otherAttributes of the list to the node
 * 
 * @param node 
 * @param otherAttributes 
 */
void addOtherAttributesToNode(xmlNodePtr node, List *otherAttributes) {
    if(node == NULL || otherAttributes == NULL) return;
    Node *head = otherAttributes->head;

    while(head) {    
        Attribute* attr = (Attribute*)head->data;
        xmlNewProp(node, BAD_CAST attr->name, BAD_CAST attr->value);
        head = head->next;
    }
}

/**
 * @brief validates each attribute struct in the otherAttributes list
 * 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool isValidAttributes(List *otherAttributes) {
    if(otherAttributes == NULL) return false;
    Node *head = otherAttributes->head;

    while(head) {    
        Attribute* attr = (Attribute*)head->data;
        
        if(attr->name == NULL){
            return false;
        }
        
        head = head->next;
    }

    return true;
}

/**
 * @brief validates each rectangle struct in the rectangles list
 * 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool isValidRectangles(List *rectangles) {
    if(rectangles == NULL) return false;
    Node *cur = rectangles->head;

    while(cur) {
        Rectangle *rect = (Rectangle*)cur->data;
    
        if(strcasecmp(rect->units, "invalid") == 0)
            return false;
        
        if(rect->height < 0 || rect->width < 0)
            return false;

        if(rect->otherAttributes == NULL) 
            return false;

        if(!isValidAttributes(rect->otherAttributes))
            return false;

        cur = cur->next;
    }
    return true;
}

/**
 * @brief validates each circles struct in the circles list
 * 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool isValidCircles(List *circles) {
    if(circles == NULL) return false;
    Node *cur = circles->head;

    while(cur) {
        Circle *c = (Circle*)cur->data;
    
        if(strcasecmp(c->units, "invalid") == 0)
            return false;
        
        if(c->r < 0)
            return false;

        if(c->otherAttributes == NULL) 
            return false;

        if(!isValidAttributes(c->otherAttributes))
            return false;

        cur = cur->next;
    }

    return true;
}

/**
 * @brief validates each path struct in the paths list
 * 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool isValidPaths(List *paths) {
    if(paths == NULL) return false;
    Node *cur = paths->head;

    while(cur) {
        Path *p = (Path*)cur->data;

        if(p->otherAttributes == NULL)
            return false;

        if(p->data == NULL)
            return false;

        if(!isValidAttributes(p->otherAttributes))
            return false;

        cur = cur->next;
    }

    return true;
}

/**
 * @brief validates each group struct in the groups list
 * 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool isValidGroups(List *groups) {
    if(groups == NULL) return false;
    Node *cur = groups->head;

    while(cur) {
        Group *g = (Group*)cur->data;

        if(!isValidAttributes(g->otherAttributes))
            return false;

        if(!isValidRectangles(g->rectangles) || !isValidCircles(g->circles) || !isValidPaths(g->paths) || !isValidGroups(g->groups))
            return false;

        cur = cur->next;
    }

    return true;
}

/**
 * @brief validates a xmlDoc (tree) against an xsd file
 * 
 * @param doc 
 * @param schemaFile 
 * @return int 
 */
int validateAgainstXSD(xmlDoc *doc, const char *schemaFile) {
    if(doc == NULL) return 1;

    if(schemaFile == NULL || schemaFile[0] == '\0' || strlen(schemaFile) == 0 || !extensionMatches(schemaFile, ".xsd"))
        return 1;

    // Read schemaFile
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    ctxt = xmlSchemaNewParserCtxt(schemaFile);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xmlSchemaValidCtxtPtr ctxtPtr;
    int ret;
    ctxtPtr = xmlSchemaNewValidCtxt(schema);

    ret = xmlSchemaValidateDoc(ctxtPtr, doc);

    xmlSchemaFreeValidCtxt(ctxtPtr);
    if(schema != NULL) xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();

    return ret;
}

/**
 * @brief updates an attribute if its already existing
 * 
 * @param attr 
 * @param otherAttributes 
 * @return true 
 * @return false 
 */
bool updateAttribute(Attribute *attr, List *otherAttributes) {
    if(attr == NULL || otherAttributes == NULL) return false;
    Node *cur = otherAttributes->head;

    while(cur) {        
        if(strcmp(((Attribute*)cur->data)->name, attr->name) == 0) {
            cur->data = realloc(cur->data, sizeof(Attribute) + (strlen(attr->value) + 1) * sizeof(char));
            strcpy(((Attribute*)cur->data)->value, attr->value);
            return true;
        }
        cur = cur->next;
    }
    return false;
}

/**
 * @brief Get the Cirle At Pos
 * 
 * @param circles 
 * @param pos 
 * @return Circle* 
 */
Circle* getCirleAtPos(List *circles, int pos) {
    Node*cur = circles->head;
    int count = 0;

    while(cur) {
        Circle* circle = (Circle*)cur->data;

        if(count == pos) return circle;

        cur = cur->next;
        count++;
    }
    return NULL;
}

/**
 * @brief Get the Rect At Pos
 * 
 * @param rectangles 
 * @param pos 
 * @return Rectangle* 
 */
Rectangle* getRectAtPos(List *rectangles, int pos) {
    Node*cur = rectangles->head;
    int count = 0;

    while(cur) {
        Rectangle* rect = (Rectangle*)cur->data;

        if(count == pos) return rect;

        cur = cur->next;
        count++;
    }
    return NULL;
}

/**
 * @brief Get the Path At Pos
 * 
 * @param paths 
 * @param pos 
 * @return Path* 
 */
Path* getPathAtPos(List *paths, int pos) {
    Node*cur = paths->head;
    int count = 0;

    while(cur) {
        Path* path = (Path*)cur->data;

        if(count == pos) return path;

        cur = cur->next;
        count++;
    }
    return NULL;
}

/**
 * @brief Get the Group At Pos
 * 
 * @param groups 
 * @param pos 
 * @return Group* 
 */
Group* getGroupAtPos(List *groups, int pos) {
    Node*cur = groups->head;
    int count = 0;

    while(cur) {
        Group* g = (Group*)cur->data;

        if(count == pos) return g;

        cur = cur->next;
        count++;
    }
    return NULL;
}

/**
 * @brief extract value out of JSON string
 * 
 * @param key 
 * @param string 
 * @return char* 
 */
char *extractValue(char *key, const char *string) {
    if(key == NULL) return NULL;

    char *str = strstr(string, key);
    if(str == NULL) return NULL;

    char *str2 = strstr(str, ":");
    if(str2 == NULL) return NULL;

    char *str3 = strstr(str2, "\"");
    if(str3 == NULL) return NULL;

    int len = 2;
    char *value = malloc(sizeof(char) * len);
    
    for(int i = 1; i < strlen(str3); i++) {
        if(str3[i] == '\"' && (str3[i+1] == ',' || str3[i+1] == '}'))
            break;
        value[i-1] = str3[i];
        len++;
        value = realloc(value, sizeof(char) * (len));
    }

    value[len-2] = '\0';

    return value;
}

/**
 * @brief extract integer value out of JSON string 
 * 
 * @param key 
 * @param string 
 * @return char* 
 */
char *extractIntValue(char *key, const char *string) {
    if(key == NULL) return NULL;

    char *str = strstr(string, key);
    if(str == NULL) return NULL;

    char *str2 = strstr(str, ":");
    if(str2 == NULL) return NULL;

    int len = 2;
    char *value = malloc(sizeof(char) * len);
    
    for(int i = 1; i < strlen(str2); i++) {
        if(str2[i] == ',' && (str2[i+1] == '\"' || str2[i+1] == '}'))
            break;
        value[i-1] = str2[i];
        len++;
        value = realloc(value, sizeof(char) * (len));
    }

    value[len-2] = '\0';
    value++;
    value[strlen(value)-1] = 0;

    return value;
}








