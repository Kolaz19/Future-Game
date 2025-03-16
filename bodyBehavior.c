#include "include/bodyBehavior.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void noUpdate(b2BodyId* body, float* status) { return; }

void setUpdateFunction(int id, void (**update)(b2BodyId*, float*)) {
	*update = &noUpdate;
}
#pragma GCC diagnostic pop
