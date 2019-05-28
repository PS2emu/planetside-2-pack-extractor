Actor definition.

Plaintext XML format that references .dme/.dma files and optionally texture overrides, [Morpheme networks & rigs](https://github.com/RoyAwesome/ps2ls/wiki/Mrn), actor sockets (see ActorSockets.xml) and some other currently unknown things.

    <ActorRuntime>
        <Skeleton fileName="[string]" scale="[float]" />
        <Base fileName="[string]" paletteName="[string]" updateRadius="[float]" waterDisplacementHeight="[float]" objectTerrainDataId="[integer]" />
        <EffectDefs>
            <Sounds>
                <SoundEmitter id="[integer]" name="[string]" boneName="[string]" heading="[float]" pitch="[float]" scale="[float]" offsetX="[float]" offsetY="[float]" offsetZ="[float]" />
            </Sounds>
            <Particles>
                <ParticleEmitter id="[integer]" name="[string]" boneName="[string]" heading="[float]" pitch="[float]" scale="[float]" offsetX="[float]" offsetY="[float]" offsetZ="[float]" sourceBoneName="[string]" localSpaceDerived="[boolean]" worldOrientation="[boolean]" />
            </Particles>
        </EffectDefs>
        <MaterialTags>
            <MaterialTag name="[string]" materialIndex="[integer]" semanticHash="[integer]" tintSetId="[integer]" defaultTintId="[integer]" />
        </MaterialTags>
        <TextureAliases>
            <Alias aliasName="[string]" textureName="[string]" modelType="[integer]" materialIndex="[integer]" semanticHash="[integer]" defaultTextureAlias="[integer]" occlusionSlotBitmask="[integer]" />
        </TextureAliases>
        <TintAliases>
            <Alias aliasName="[string]" textureName="[string]" red="[float]" green="[float]" blue="[float]" modelType="[integer]" materialIndex="[integer]" semanticHash="[integer]" defaultTintAlias="[integer]" />
        </TintAliases>
        <EffectList>
            <Effect toolName="[string]" type="[string]" name="[string]" id="[integer]" />
        </EffectList>
        <Lods>
            <Lod fileName="[string]" distance="[float]" />
            <Lod0a distance="[float]" />
        </Lods>
        <Animations>
            <Animation name="[string]" fileName="[string]" playbackScale="[float]" animationDurationSec="[float]" loadType="[integer]" required="[0|1]" effectAlwaysVisible="[boolean]" />
        </Animations>
        <AnimationSounds>
            <Animation name="[string]">
                <Effect type="[string]" name="[string]" toolName="[string]" id="[integer]" playOnce="[boolean]" loadType="[integer]">
                    <TriggerEvent name="[string]" startTime="[float]" endTime="[float]" />
                </Effect>
            </Animation>
        </AnimationSounds>
        <AnimationParticles>
            <Animation name="[string]">
                <Effect type="[string]" name="[string]" toolName="[string]" id="[integer]" playOnce="[boolean]" loadType="[integer]">
                    <TriggerEvent name="[string]" startTime="[float]" endTime="[float]" />
                </Effect>
            </Animation>
        </AnimationParticles>
        <AnimationActionPoints>
            <Animation name="[string]">
                <ActionPoints>
                    <ActionPoint name="[string]" time="[float]" />
                </ActionPoints>
            </Animation>
        </AnimationActionPoints>
        <AnimationCompositeEffects>
            <Animation name="[string]">
                <Effect type="[string]" name="[string]" toolName="[string]" id="[integer]" playOnce="[boolean]" loadType="[integer]">
                    <TriggerEvent name="[string]" startTime="[float]" endTime="[float]" />
                </Effect>
            </Animation>
        </AnimationCompositeEffects>
        <CollisionData fileName="[string]" />
        <Slots OcclusionSlots="[integer]">
            <CoveredSlots name="[string]" />
        </Slots>
        <Usage actorUsage="[integer]" borrowSkeleton="[boolean]" boneAttachmentName="[string]" replicationBoneName="[string]" validatePcNpc="[0|1]" inheritAnimations="[boolean]" />
        <HatHair type="[string]" coversFacialHair="[boolean]" />
        <Shadows CheckShadowVisibility="[boolean]" />
        <EquipType EquippedSlotName="[string]" ParentAttachSlot="[string]" EquipType="[integer]" EquipTypeSelection="[integer]" ChildAttachSlot="[string]" />
        <BoneMetadata name="[string]" joint="[string]" joint2="[string]" collisionType="[integer]" />
        <Mountable MinOccupancy="[integer]" AnimSlotPrefix="[string]" IdleAnim="[string]" IdleToRunAnim="[string]" RunAnim="[string]" RunToIdleAnim="[string]">
            <Seat Bone="[string]" Anim="[string]" Controller="[boolean]" RiderMimicsSeatAnim="[boolean]" >
                <Entrance Bone="[string]" Anim="[string]" Location="[string]" />
                <Exit Bone="[string]" Anim="[string]" Location="[string]" />
            </Seat>
        </Mountable>
        <LookControls>
            <LookControl name="[string]" type="[integer]" effectorBone="[string]">
                <JointEntry bone="[string]" pitchLimit="[float]" yawLimit="[float]" turnRate="[float]" />
            </LookControl>
        </LookControls>
        <AnimationNetwork fileName="[string]" animSetName="[string]" />
        <CollisionType type="[integer]" />
        <ChildAttachSlotsEx>
            <ChildAttachSlotEx slotName="[string]" />
        </ChildAttachSlotsEx>
        <OcclusionData BlockerModel="[string]" TestModel="[string]" />
        <AnimationEnumerations>
            <AnimationEnumeration hash="[integer]" value="[integer]" />
        </AnimationEnumerations>
        <AnimationValues>
            <AnimationValue hash="[integer]" value="[integer]" />
        </AnimationValues>
        <MaterialType type="[string]" />
        <Invisible value="[boolean]" />
        <DecalTintTranslations>
            <Translation LocationName="[string]" xUvLocation="[float]" yUvLocation="[float]" UV="[boolean]" scaleXVal="[float]" scaleYVal="[float]" rotation="[float]" />
        </DecalTintTranslations>
    </ActorRuntime>
