/////////////////////////////////////////////////////////////////////////////
// Purpose:   Maintain a undo and redo stack
// Author:    Ralph Walden
// Copyright: Copyright (c) 2021 KeyWorks Software (Ralph Walden)
// License:   Apache License -- see ../LICENSE
/////////////////////////////////////////////////////////////////////////////

#pragma once

class UndoAction
{
public:
    UndoAction(ttlib::sview undo_string = tt_empty_cstr)
    {
        if (undo_string.size())
        {
            m_undo_string = undo_string;
        }
    }

    virtual ~UndoAction() = default;

    // Called when pushed to the Undo stack and when Redo is called
    virtual void Change() = 0;

    // Called when Undo is requested
    virtual void Revert() = 0;

    ttlib::cstr GetUndoString() { return m_undo_string; }
    void SetUndoString(ttlib::sview str) { m_undo_string = str; }

    bool wasUndoEventGenerated() { return m_UndoEventGenerated; }
    bool wasRedoEventGenerated() { return m_RedoEventGenerated; }
    bool wasUndoSelectEventGenerated() { return m_UndoSelectEventGenerated; }
    bool wasRedoSelectEventGenerated() { return m_RedoSelectEventGenerated; }

protected:
    ttlib::cstr m_undo_string;

    bool m_UndoEventGenerated { false };
    bool m_RedoEventGenerated { false };
    bool m_UndoSelectEventGenerated { false };
    bool m_RedoSelectEventGenerated { false };
};

using UndoActionPtr = std::shared_ptr<UndoAction>;

class UndoStack
{
public:
    // This will first call UndoAction->Change(), then clear the redo stack and push the
    // UndoAction onto the undo stack
    void Push(UndoActionPtr ptr);

    // This will pop the last UndoAction, call UndoAction->Revert() and then push the
    // command on the redo stack
    void Undo();

    // This will pop the last UndoAction from the redo stack, call UndoAction->Change()
    // and then push the command on the undo stack
    void Redo();

    // When the stack is locked, Push() will call UndoAction->Change(), but will not change
    // the undo or redo stacks
    void Lock() { m_locked = true; }
    void Unlock() { m_locked = false; }
    bool IsLocked() { return m_locked; }

    bool IsUndoAvailable() { return m_undo.size(); }
    bool IsRedoAvailable() { return m_redo.size(); }

    wxString GetUndoString();
    wxString GetRedoString();

    size_t size() { return m_undo.size(); }

    void clear()
    {
        m_redo.clear();
        m_undo.clear();
    }

    // When undo is called, the command is popped and pushed onto the redo stack. So to get at the last undo command, you
    // have to get the last item in the redo stack. Redo works just the opposite, pushing it's command to the last of the
    // undo stack.

    bool wasUndoEventGenerated() { return m_redo.back()->wasUndoEventGenerated(); }
    bool wasRedoEventGenerated() { return m_undo.back()->wasRedoEventGenerated(); }
    bool wasUndoSelectEventGenerated() { return m_redo.back()->wasUndoSelectEventGenerated(); }
    bool wasRedoSelectEventGenerated() { return m_undo.back()->wasRedoSelectEventGenerated(); }

private:
    std::vector<UndoActionPtr> m_undo;
    std::vector<UndoActionPtr> m_redo;

    bool m_locked { false };
};
