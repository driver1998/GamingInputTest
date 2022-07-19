#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <windef.h>
#include <windows.h>

void LVAddColumn(HWND hwnd, int pos, const wchar_t* header, int width);
void LVSetColumnWidth(HWND hwnd, int iCol, int width);
void LVSetItemText(HWND hwnd, int iItem, int iSubItem, const wchar_t* text);
void LVSetItemGroupId(HWND hwnd, int iItem, int groupId);
void LVSetItemIndent(HWND hwnd, int iItem, int indent);
void LVSetItemImage(HWND hwnd, int iItem, int imageId);
void LVSetItemParam(HWND hwnd, int iItem, LPARAM param);
void LVAddGroup(HWND hwnd, int pos, const wchar_t* header, int id);
void LVDeleteItem(HWND hwnd, int pos);
int LVAddItem(HWND hwnd, int pos, const wchar_t* text, LPARAM param);
int LVFindItemByParam(HWND hwnd, int startIndex, LPARAM param);
#endif