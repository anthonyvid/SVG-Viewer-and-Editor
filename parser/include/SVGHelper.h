/**
 * @file SVGHelper.h
 * @author Anthony Vidovic (1130891)
 * @brief Header file for helper functions
 * @version 0.1
 * @date 2022-02-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SVGHelper
#define SVGHelper

// ~~~~~ Includes ~~~~~ //
#include <stdlib.h>
#include "SVGParser.h"
#include <ctype.h>
#include <strings.h>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* ----------------------- */
/* Assignment 1 Prototypes */
/* ----------------------- */

// ~~~~~ Helper Prototypes for module 1 ~~~~~ //
void addElementsToSVG(SVG* svg, xmlNode* a_node);
char *removeAllExceptDigits(char *string);
char *prependChar(char before, char *str);
char *checkForUnits(char *string);
void iterateGroup(xmlNode* a_node, Group *g);
Attribute *newAttribute(xmlAttr* attribute);
Rectangle* createRectangle(xmlNode* cur_node, xmlAttr* attribute);
Circle* createCircle(xmlNode* cur_node, xmlAttr* attribute);
Path* createPath(xmlNode* cur_node, xmlAttr* attribute);
Group* createGroup(xmlNode* cur_node, xmlAttr* attribute);

// ~~~~~ Helper Prototypes for module 2 ~~~~~ //
void findRectanglesInGroup(List *group, List *rectangles);
void findCirclesInGroup(List *group, List *circles);
void findPathsInGroup(List *group, List *paths);
void findGroups(List *group, List *groups);
void searchGroupsForRectArea(List *group, int *found, float area);
void searchGroupsForCircleArea(List *group, int *found, float area);
void searchGroupsForPathData(List *group, int *found, const char *data);
void searchGroupsForGroupLen(List *group, int *found, int len);
void searchGroupsForAttributes(List *group, int *found);
void dummyDelete(void* data);

/* ----------------------- */
/* Assignment 2 Prototypes */
/* ----------------------- */
xmlDoc* SVGtoDOC(const SVG *svg);
void addOtherAttributesToNode(xmlNodePtr node, List *otherAttributes);
void addRectanglesToParent(xmlNodePtr node, List *rectangles);
void addCirclesToParent(xmlNodePtr node, List *circles);
void addPathsToParent(xmlNodePtr node, List *paths);
void addGroupsToParent(xmlNodePtr node, List *groups);
char* removeDigitsFromStr(char *stringToStrip);
bool extensionMatches(const char *fileName, char *extension);
bool isValidRectangles(List *rectangles);
bool isValidCircles(List *circles);
bool isValidPaths(List *paths);
bool isValidGroups(List *groups);
bool isValidAttributes(List *attributes);
int validateAgainstXSD(xmlDoc *doc, const char *schemaFile);
bool updateAttribute(Attribute*attr, List *otherAttributes);
Circle* getCirleAtPos(List *circles, int pos);
Rectangle* getRectAtPos(List *rectangles, int pos);
Path* getPathAtPos(List *paths, int pos);
Group* getGroupAtPos(List *groups, int pos);
char *extractValue(char *key, const char *string);
char *extractIntValue(char *key, const char *string);

/* ----------------------- */
/* Assignment 3 Prototypes */
/* ----------------------- */
char *createSVGWrapper(char *filename, char *schemaFile);
bool validateSVGWrapper(char *filename, char *schemaFile);
char *getSVGRects(char *filename, char *schemaFile);
char *getSVGCircs(char *filename, char *schemaFile);
char *getSVGPaths(char *filename, char *schemaFile);
char *getSVGGroups(char *filename, char *schemaFile);
char *getSVGTitleAndDesc(char *filename, char *schemaFile);

char *getRectOtherAttributes(char *filename, char *schemaFile);
char *getCircOtherAttributes(char *filename, char *schemaFile);
char *getPathOtherAttributes(char *filename, char *schemaFile);
char *getGroupOtherAttributes(char *filename, char *schemaFile);
bool setAttributeWrapper(char *filename, char *schemaFile, char *name, char *value, int index, int elementType);
bool addComponentWrapper(char *filename ,char *schemaFile, int elementType, char *json);
bool scaleShape(char *filename, char *schemaFile, int elementType, int scaleVal);
void scaleRectsInGroups(int scaleVal, List *groups);
void scaleCircsInGroups(int scaleVal, List *groups);
bool createNewSVG(char *filename, char *schemaFile, char *json);

#endif
