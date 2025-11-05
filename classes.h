//
// Created by Silvitio on 05.11.2025.
//

#ifndef GOLANG_COMPILER_CLASSES_H
#define GOLANG_COMPILER_CLASSES_H

class AstNode {
protected:
    static unsigned int maxId;
    unsigned int id;

public:
    AstNode() {id = ++maxId;};
};

#endif //GOLANG_COMPILER_CLASSES_H