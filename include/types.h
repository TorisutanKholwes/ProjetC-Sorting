/*
 * Copyright (c) 2025 Torisutan
 * ALl rights reserved
 */

#pragma once

// Pointer declarations of all types used across the application
/** @brief Application main structure */
typedef struct App App;

/** @brief Event handling structure */
typedef struct Input Input;

/** @brief Event handler structure */
typedef struct EventHandler EventHandler;

/** @brief Node of a doubly linked list */
typedef struct ListNode ListNode;
/** @brief Doubly linked list structure */
typedef struct List List;
/** @brief Iterator for the doubly linked list */
typedef struct ListIterator ListIterator;
/** @brief Types of sorting for the list */
typedef enum ListSortType ListSortType;

/** @brief Log levels for logging system */
typedef enum LogLevel LogLevel;

/** @brief Node of a map (key-value pair) */
typedef struct MapNode MapNode;
/** @brief Map structure for key-value storage */
typedef struct Map Map;
/** @brief Iterator for the map structure */
typedef struct MapIterator MapIterator;

/** @brief String builder for efficient string manipulation */
typedef struct StringBuilder StringBuilder;

/** @brief 2D position structure */
typedef struct Position Position;
/** @brief RGBA color structure */
typedef struct Color Color;

/** @brief Types of UI elements */
typedef enum ElementType ElementType;
/** @brief Base structure for all UI elements */
typedef struct Element Element;

/** @brief Button UI element */
typedef struct Button Button;
/** @brief Text UI element */
typedef struct Text Text;
/** @brief Input box UI element */
typedef struct InputBox InputBox;

/** @brief Resource manager for loadiing and caching assets */
typedef struct ResourceManager ResourceManager;

/** @brief left, top, right, bottom insets structure */
typedef struct EdgeInsets EdgeInsets;
/** @brief Text style structure */
typedef struct TextStyle TextStyle;
/** @brief Full style colors structure */
typedef struct FullStyleColors FullStyleColors;
/* @brief Button style structure */
typedef struct ButtonStyle ButtonStyle;
/* @brief Input box style structure */
typedef struct InputBoxStyle InputBoxStyle;
/* @brief Theme structure */
typedef struct Theme Theme;

/* @brief Frame structure */
typedef struct Frame Frame;

/** @brief A UI layout box */
typedef struct Box Box;
/** @brief A UI layout Circle */
typedef struct Circle Circle;
/** @brief A UI layout Polygon */
typedef struct Polygon Polygon;

/** @brief Timer structure for measuring elapsed time */
typedef struct Timer Timer;

/** @brief FlexContainer for flexible box layout */
typedef struct FlexContainer FlexContainer;
/** @brief FlexItem for items within a FlexContainer */
typedef struct FlexItem FlexItem;
/** @brief Flexbox direction enumeration */
typedef enum FlexDirection FlexDirection;
/** @brief Flexbox justification enumeration */
typedef enum FlexJustify FlexJustify;
/** @brief Flexbox alignment enumeration */
typedef enum FlexAlign FlexAlign;

/** @brief UI layout Image */
typedef struct Image Image;

/** @brief A column graph structure */
typedef struct ColumnGraph ColumnGraph;
/** @brief A bar within a column graph */
typedef struct ColumnGraphBar ColumnGraphBar;

/** @brief A container for grouping UI elements */
typedef struct Container Container;

/** @brief A selectable UI element */
typedef struct Select Select;

// Frames
/** @brief The main application frame */
typedef struct MainFrame MainFrame;
/** @brief The help frame */
typedef struct HelpFrame HelpFrame;

// Structure who's not used as a pointer elsewhere
/** @brief 2D size structure */
typedef struct {
    float width, height;
} Size;

/** @brief Column graph styles (colors) */
typedef enum ColumnGraphStyle {
    GRAPH_RAINBOW,
    GRAPH_WHITE,
    GRAPH_BLACK,
    GRAPH_GRAY,
    GRAPH_BLACK_AND_WHITE,
    GRAPH_STYLE_COUNT
} ColumnGraphStyle;

/** @brief Column graph data types */
typedef enum ColumnGraphType {
    GRAPH_TYPE_INT,
    GRAPH_TYPE_STRING
} ColumnGraphType;

/** @brief Types of sorting for the list */
typedef enum ListSortType {
    LIST_SORT_TYPE_BUBBLE,
    LIST_SORT_TYPE_QUICK,
    LIST_SORT_TYPE_MERGE,
    LIST_SORT_TYPE_COUNT
} ListSortType;


// Types of func
/** @brief Function pointer type for event handling */
typedef void (*EventHandlerFunc)(Input* input, SDL_Event* event, void* data);

/** @brief Function pointer for focus and unfocus */
typedef void (*FrameFocusFunc)(void* data);
/** @brief Function pointer for frame update and render */
typedef void (*FrameUpdateFunc)(void* data);
/** @brief Function pointer for frame rendering */
typedef void (*FrameRenderFunc)(SDL_Renderer* renderer, void* data);

/** @brief Function pointer for destroying data */
typedef void (*DestroyFunc)(void* data);

/** @brief Function pointer for hover detection in column graphs */
typedef bool (*ColumnsHoverFunc)(void* data, void* value, ColumnGraphType type);

/** @brief Function pointer for comparing two elements */
typedef int (*CompareFunc)(const void* a, const void* b);

typedef void (*DelayFunc)(MainFrame* self, ColumnGraph* graph, ColumnGraphBar* actual);