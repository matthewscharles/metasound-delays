#include "MetasoundEchoes/Public/MetasoundClickNode.h"
#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"                 // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_ClickNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace ClickNodeNames
    {
        METASOUND_PARAM(InputTrigger, "Trigger", "Trigger input to generate an impulse.");
        METASOUND_PARAM(InputBiPolar, "Bi-Polar", "Toggle between bipolar and unipolar impulse output.");
        METASOUND_PARAM(OutputOnTrigger, "On Trigger", "Trigger output when the node is triggered.");
        METASOUND_PARAM(OutputImpulse, "Impulse Out", "Generated impulse output.");
    }

    // Operator Class - defines the way the node is described, created and executed
    class FClickOperator : public TExecutableOperator<FClickOperator>
    {
    public:
        // Constructor
        FClickOperator(
            const FOperatorSettings& InSettings,
            const FTriggerReadRef& InTrigger,
            const FBoolReadRef& InBiPolar)
            : InputTrigger(InTrigger)
            , InputBiPolar(InBiPolar)
            , OnTrigger(FTriggerWriteRef::CreateNew(InSettings))
            , OutputImpulse(FAudioBufferWriteRef::CreateNew(InSettings))
            , SignalIsPositive(true)
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace ClickNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputTrigger)),
                    TInputDataVertexModel<bool>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputBiPolar), true)
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FTrigger>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputOnTrigger)),
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputImpulse))
                )
            );

            return Interface;
        }

        // Retrieves necessary metadata about the node
        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
                {
                    FVertexInterface NodeInterface = DeclareVertexInterface();

                    FNodeClassMetadata Metadata;

                    Metadata.ClassName = { StandardNodes::Namespace, TEXT("Click"), StandardNodes::AudioVariant };
                    Metadata.MajorVersion = 1;
                    Metadata.MinorVersion = 0;
                    Metadata.DisplayName = METASOUND_LOCTEXT("ClickNodeDisplayName", "Click");
                    Metadata.Description = METASOUND_LOCTEXT("ClickNodeDesc", "Generates a single-sample impulse when triggered.");
                    Metadata.Author = "Charles Matthews";
                    Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                    Metadata.DefaultInterface = DeclareVertexInterface();
                    Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("Custom", "Delays") };
                    Metadata.Keywords = TArray<FText>();

                    return Metadata;
                };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        // Allows MetaSound graph to interact with the node's inputs
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace ClickNodeNames;
            FDataReferenceCollection Inputs;
            Inputs.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputTrigger), InputTrigger);
            Inputs.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputBiPolar), InputBiPolar);
            return Inputs;
        }

        // Allows MetaSound graph to interact with the node's outputs
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace ClickNodeNames;

            FDataReferenceCollection OutputDataReferences;
            
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputOnTrigger), OnTrigger);
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputImpulse), OutputImpulse);

            return OutputDataReferences;
        }

        // Used to instantiate a new runtime instance of the node
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace ClickNodeNames;

            const Metasound::FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const Metasound::FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            TDataReadReference<FTrigger> InputTrigger = InputCollection.GetDataReadReferenceOrConstruct<FTrigger>(METASOUND_GET_PARAM_NAME(InputTrigger), InParams.OperatorSettings);
            TDataReadReference<bool> InputBiPolar = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<bool>(InputInterface, METASOUND_GET_PARAM_NAME(InputBiPolar), InParams.OperatorSettings);

            return MakeUnique<FClickOperator>(
                InParams.OperatorSettings,
                InputTrigger,
                InputBiPolar
            );
        }

        // Primary node functionality
        void Execute()
        {
            OnTrigger->AdvanceBlock();
            OutputImpulse->Zero(); // Ensure the output buffer is cleared
            
            // Initialize the output buffer to zero
            int32 NumFrames = OutputImpulse->Num();
            float* OutputDataPtr = OutputImpulse->GetData();
            FMemory::Memzero(OutputDataPtr, sizeof(float) * NumFrames);

            // Process trigger events
            InputTrigger->ExecuteBlock(
                // Pre-trigger lambda (called before any triggers in the block)
                [](int32 StartFrame, int32 EndFrame)
                {
                    // No action needed before triggers
                },

                // On-trigger lambda (called for each trigger event)
                [&](int32 TriggerFrame, int32 TriggerFrameEnd)
                {
                    if (TriggerFrame < NumFrames)
                    {
                        OnTrigger->TriggerFrame(TriggerFrame);
                        if (*InputBiPolar)
                        {
                            OutputDataPtr[TriggerFrame] = SignalIsPositive ? 1.0f : -1.0f;
                            SignalIsPositive = !SignalIsPositive;
                        }
                        else
                        {
                            OutputDataPtr[TriggerFrame] = 1.0f;
                        }
                    }
                }
            );
        }

    private:

        // Inputs
        FTriggerReadRef InputTrigger;
        FBoolReadRef InputBiPolar;

        // Outputs
        FTriggerWriteRef OnTrigger;
        FAudioBufferWriteRef OutputImpulse;

        bool SignalIsPositive;

    };

    // Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
    class FClickNode : public FNodeFacade
    {
    public:
        FClickNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FClickOperator>())
        {
        }
    };

    // Register node
    METASOUND_REGISTER_NODE(FClickNode);
}

#undef LOCTEXT_NAMESPACE