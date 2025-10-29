/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

// Pointer declarations of all types used across the application
typedef struct App App;

typedef struct Input Input;
typedef struct EventHandler EventHandler;

typedef struct ListNode ListNode;
typedef struct List List;
typedef struct ListIterator ListIterator;
typedef enum ListSortType ListSortType;

typedef enum LogLevel LogLevel;

typedef struct MapNode MapNode;
typedef struct Map Map;
typedef struct MapIterator MapIterator;

typedef struct StringBuilder StringBuilder;

typedef struct Position Position;
typedef struct Color Color;

typedef enum ElementType ElementType;
typedef struct Element Element;

typedef struct Button Button;
typedef struct Text Text;
typedef struct InputBox InputBox;

typedef struct ResourceManager ResourceManager;

typedef struct EdgeInsets EdgeInsets;
typedef struct TextStyle TextStyle;
typedef struct FullStyleColors FullStyleColors;
typedef struct ButtonStyle ButtonStyle;
typedef struct InputBoxStyle InputBoxStyle;
typedef struct Theme Theme;

typedef struct Frame Frame;

typedef struct Box Box;
typedef struct Circle Circle;
typedef struct Polygon Polygon;

typedef struct Timer Timer;

typedef struct FlexContainer FlexContainer;
typedef struct FlexItem FlexItem;
typedef enum FlexDirection FlexDirection;
typedef enum FlexJustify FlexJustify;
typedef enum FlexAlign FlexAlign;

typedef struct Image Image;

typedef struct ColumnGraph ColumnGraph;
typedef struct ColumnGraphBar ColumnGraphBar;

typedef struct Container Container;

typedef struct Select Select;

// Frames
typedef struct MainFrame MainFrame;
typedef struct HelpFrame HelpFrame;

// Structure who's not used as a pointer elsewhere
typedef struct {
    float width, height;
} Size;

typedef enum ColumnGraphStyle {
    GRAPH_RAINBOW,
    GRAPH_WHITE,
    GRAPH_BLACK,
    GRAPH_GRAY,
    GRAPH_BLACK_AND_WHITE,
    GRAPH_STYLE_COUNT
} ColumnGraphStyle;

// Types of func
typedef void (*EventHandlerFunc)(Input* input, SDL_Event* event, void* data);

typedef void (*FrameFocusFunc)(void* data);
typedef void (*FrameUpdateFunc)(void* data);
typedef void (*FrameRenderFunc)(SDL_Renderer* renderer, void* data);

typedef void (*DestroyFunc)(void* data);

typedef bool (*ColumnsHoverFunc)(void* data, float value);

typedef int (*CompareFunc)(const void* a, const void* b);