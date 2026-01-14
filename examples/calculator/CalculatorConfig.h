#ifndef CALCULATORCONFIG_H
#define CALCULATORCONFIG_H

enum class NodeOpCode : int {
    Input  = 1,
    Output = 2,
    Add    = 3,
    Sub    = 4,
    Mul    = 5,
    Div    = 6
};

constexpr const char* LISTBOX_MIMETYPE = "application/x-item";


#endif // CALCULATORCONFIG_H
