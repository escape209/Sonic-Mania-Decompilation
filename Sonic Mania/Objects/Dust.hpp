#ifndef OBJ_DUST_H
#define OBJ_DUST_H

#include "../SonicMania.hpp"

// Object Class
struct ObjectDust : Object {
    ushort spriteIndex;
};

// Entity Class
struct EntityDust : Entity {
    void (*state)();
    int timer;
    Entity *parent;
    EntityAnimationData data;
};

// Object Struct
extern ObjectDust *Dust;

// Standard Entity Events
void Dust_Update();
void Dust_LateUpdate();
void Dust_StaticUpdate();
void Dust_Draw();
void Dust_Create(void* data);
void Dust_StageLoad();
void Dust_EditorDraw();
void Dust_EditorLoad();
void Dust_Serialize();

// Extra Entity Functions
void Dust_State_DropDash();
void Dust_State_HammerDrop();
void Dust_State_GlideSlide();
void Dust_State_Skid();
void Dust_State_Spindash();
void Dust_State_EggLoco();

#endif //!OBJ_DUST_H
