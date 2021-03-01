#include "../SonicMania.hpp"

ObjectSpring *Spring;

void Spring_Update()
{
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;

    if (entity->timer > 0)
        entity->timer--;
    if (entity->state)
        entity->state();
    RSDK.ProcessAnimation(&entity->data);
    if (entity->data.frameID == 8)
        entity->data.animationSpeed = 0;
}

void Spring_LateUpdate() {}

void Spring_StaticUpdate() {}

void Spring_Draw()
{
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;
    RSDK.DrawSprite(&entity->data, NULL, false);
}

void Spring_Create(void *data)
{
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;
    entity->drawFX       = FX_FLIP;
    if (!RSDK_sceneInfo->inEditor) {
        entity->type %= 6;
        if (data) {
            int *pv          = (int *)data;
            entity->type     = (*pv >> 0) & 0xFF;
            entity->flipFlag = (*pv >> 8) & 0xFF;
        }
        RSDK.SetSpriteAnimation(Spring->spriteIndex, entity->type, &entity->data, true, 0);
        entity->active              = ACTIVE_BOUNDS;
        entity->data.animationSpeed = 0;
        entity->updateRange.x       = 0x600000;
        entity->updateRange.y       = 0x600000;
        entity->visible             = true;
        if (entity->planeFilter && ((byte)entity->planeFilter - 1) & 2)
            entity->drawOrder = Zone->drawOrderHigh;
        else
            entity->drawOrder = Zone->drawOrderLow;

        if (entity->type >> 1) {
            if (entity->type >> 1 == 1) {
                entity->direction = entity->flipFlag;
                if (entity->type & 1)
                    entity->velocity.x = 0x100000;
                else
                    entity->velocity.x = 0xA0000;
                if (entity->flipFlag)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -0x8;
                entity->hitbox.top    = -0xF;
                entity->hitbox.right  = 0x10;
                entity->hitbox.bottom = 0x8;
                entity->state         = Spring_Unknown2;
            }
            else if (entity->type >> 1 == 2) {
                entity->direction = entity->flipFlag;
                if (entity->type & 1) {
                    entity->velocity.x = 0xB4000;
                    entity->velocity.y = 0xB4000;
                }
                else {
                    entity->velocity.x = 0x74000;
                    entity->velocity.y = 0x74000;
                }
                if (entity->flipFlag < 2u)
                    entity->velocity.y = -entity->velocity.y;
                if (entity->flipFlag & 1)
                    entity->velocity.x = -entity->velocity.x;
                entity->hitbox.left   = -0xC;
                entity->hitbox.top    = -0xB;
                entity->hitbox.right  = 0xC;
                entity->hitbox.bottom = 0xC;
                entity->state         = Spring_Unknown3;
            }
        }
        else {
            entity->direction = entity->flipFlag;
            if (entity->type & 1)
                entity->velocity.y = 0x100000;
            else
                entity->velocity.y = 0xA0000;
            if (!entity->flipFlag)
                entity->velocity.y = -entity->velocity.y;
            entity->hitbox.left   = -0x10;
            entity->hitbox.top    = -0x7;
            entity->hitbox.right  = 0x10;
            entity->hitbox.bottom = 0x8;
            entity->state         = Spring_Unknown1;
        }
    }
}

void Spring_StageLoad()
{
    Spring->spriteIndex = RSDK.LoadAnimation("Global/Springs.bin", SCOPE_STAGE);
    Spring->sfx_Spring  = RSDK.GetSFX("Global/Spring.wav");
}

void Spring_Unknown1()
{
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;
    EntityPlayer *player = NULL;
    if (RSDK_sceneInfo->entity->direction == FLIP_NONE) {
        bool32 result = false;
        for (result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player); result;
             result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player)) {
            if (!entity->planeFilter || player->collisionPlane == ((entity->planeFilter - 1) & 1)) {
                int col = 0;
                entity->type != 0xFF || player->velocity.y >= -0x50000 ? Player_CheckCollisionBox(player, entity, &entity->hitbox)
                                                                       : Player_CheckCollisionPlatform(player, entity, &entity->hitbox);
                if (col == 1) {
                    int anim = player->playerAnimData.animationID;
                    if (anim == ANI_WALK || anim > ANI_AIRWALK && anim <= ANI_DASH)
                        player->storedAnim = player->playerAnimData.animationID;
                    else
                        player->storedAnim = ANI_WALK;

                    //if (player->state != Ice_StateFrozenPlayer) {
                        if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll) {
                            player->state = Player_State_RollLock;
                        }
                        else {
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGTWIRL, &player->playerAnimData, true, 0);
                            player->state = Player_State_Air;
                        }
                    //}
                    player->onGround                        = 0;
                    player->velocity.y                      = entity->velocity.y;
                    player->tileCollisions                  = 1;
                    entity->data.animationSpeed             = 0x80;
                    entity->data.animationTimer             = 0;
                    entity->data.frameID                    = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
    else {
        bool32 result = false;
        for (result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player); result;
             result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player)) {
            if ((!entity->planeFilter || player->collisionPlane == ((entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 4) {
                //if (player->state != Ice_StateFrozenPlayer) {
                    if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll)
                        player->state = Player_State_RollLock;
                    else
                        player->state = Player_State_Air;
                //}
                player->onGround                        = 0;
                player->velocity.y                      = entity->velocity.y;
                player->tileCollisions                  = 1;
                entity->data.animationSpeed             = 0x80;
                entity->data.animationTimer             = 0;
                entity->data.frameID                    = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_Unknown2()
{
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;
    EntityPlayer *player = 0;
    if (entity->direction == FLIP_NONE) {
        bool32 result = false;
        for (result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player); result;
             result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player)) {
            if ((!entity->planeFilter || player->collisionPlane == ((entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 3 && (!entity->onGround || player->onGround)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                //if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                        if (player->onGround == 1)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int anim = player->playerAnimData.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimData.animationID = ANI_WALK;
                //}
                player->glideTimer          = 16;
                player->skidding            = 0;
                player->pushing             = 0;
                player->direction           = 0;
                player->tileCollisions      = 1;
                entity->data.animationSpeed = 0x80;
                entity->data.animationTimer = 0;
                entity->data.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
    else {
        bool32 result = false;
        for (result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player); result;
             result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player)) {

            if ((!entity->planeFilter || player->collisionPlane == ((entity->planeFilter - 1) & 1))
                && Player_CheckCollisionBox(player, entity, &entity->hitbox) == 2 && (entity->onGround == false || player->onGround == 1)) {
                if (player->collisionMode == CMODE_ROOF) {
                    player->velocity.x = -entity->velocity.x;
                    player->groundVel  = -entity->velocity.x;
                }
                else {
                    player->velocity.x = entity->velocity.x;
                    player->groundVel  = player->velocity.x;
                }

                //if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state != Player_State_Roll && player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                        if (player->onGround)
                            player->state = Player_State_Ground;
                        else
                            player->state = Player_State_Air;
                    }
                    int anim = player->playerAnimData.animationID;
                    if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                        player->playerAnimData.animationID = ANI_WALK;
                //}
                player->glideTimer          = 16;
                player->skidding            = 0;
                player->pushing             = 0;
                player->direction           = 1;
                player->tileCollisions      = 1;
                entity->data.animationSpeed = 0x80;
                entity->data.animationTimer = 0;
                entity->data.frameID        = 1;
                if (entity->timer == 0) {
                    RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                    entity->timer = 8;
                }
            }
        }
    }
}
void Spring_Unknown3()
{
    EntityPlayer *player = NULL;
    EntitySpring *entity = (EntitySpring *)RSDK_sceneInfo->entity;
    bool32 result        = false;
    for (result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player); result;
         result = RSDK.GetActiveObjects(Player->objectID, (Entity **)&player)) {
        if ((!entity->planeFilter || player->collisionPlane == ((entity->planeFilter - 1) & 1)) && !player->field_1F0) {
            if (Player_CheckCollisionTouch(player, entity, &entity->hitbox)) {
                bool flag = false;
                if (player->onGround) {
                    flag = true;
                }
                else {
                    int y = player->velocity.y;
                    if (y >= 0) {
                        flag = true;
                    }
                    else {
                        int x = player->velocity.x;
                        if (x < 0)
                            x = -x;
                        flag = x > -y;
                    }
                }
                if (flag) {
                    // if (player->state != Ice_State_FrozenPlayer) {
                    if (player->state == Player_State_RollLock || player->state == Player_State_ForceRoll) {
                        player->state = Player_State_RollLock;
                    }
                    else {
                        player->state = Player_State_Air;
                        int anim      = player->playerAnimData.animationID;
                        if (anim != ANI_JUMP && anim != ANI_JOG && anim != ANI_RUN && anim != ANI_DASH)
                            player->playerAnimData.animationID = ANI_WALK;
                    }
                    //}
                    if (entity->direction < FLIP_Y) {
                        if (player->state != Player_State_RollLock && player->state != Player_State_ForceRoll) {
                            int anim = player->playerAnimData.animationID;
                            if (anim == ANI_WALK || anim > ANI_AIRWALK && anim <= ANI_DASH)
                                player->storedAnim = player->playerAnimData.animationID;
                            else
                                player->storedAnim = ANI_WALK;
                            RSDK.SetSpriteAnimation(player->spriteIndex, ANI_SPRINGDIAGONAL, &player->playerAnimData, true, 0);
                        }
                    }
                    player->direction           = entity->direction & 1;
                    player->onGround            = 0;
                    player->velocity.x          = entity->velocity.x;
                    player->velocity.y          = entity->velocity.y;
                    player->tileCollisions      = 1;
                    entity->data.animationSpeed = 0x80;
                    entity->data.animationTimer = 0;
                    entity->data.frameID        = 1;
                    if (entity->timer == 0) {
                        RSDK.PlaySFX(Spring->sfx_Spring, 0, 255);
                        entity->timer = 8;
                    }
                }
            }
        }
    }
}

void Spring_EditorDraw() {}

void Spring_EditorLoad() {}

void Spring_Serialize()
{
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, type);
    RSDK_EDITABLE_VAR(Spring, VAR_ENUM, flipFlag);
    RSDK_EDITABLE_VAR(Spring, VAR_BOOL, onGround);
    RSDK_EDITABLE_VAR(Spring, VAR_UINT8, planeFilter);
}
