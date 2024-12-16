#include "MetasoundEchoes/Public/MetasoundTesterNode.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundPrimitives.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundFacade.h"
#include "MetasoundParamHelper.h"

#define LOCTEXT_NAMESPACE "MetasoundTesterNode"

namespace Metasound
{
    // Vertex Names - define the node's inputs and outputs here
    namespace TesterNodeNames
    {
        METASOUND_PARAM(InputSignal, "In", "Input audio signal.");
        METASOUND_PARAM(OutputSignal, "Out", "Output audio signal.");
    }

    // Operator Class - defines the way the node is described, created, and executed
    class FTesterOperator : public TExecutableOperator<FTesterOperator>
    {
    public:
        // Constructor
        FTesterOperator(
            const FOperatorSettings& InSettings,
            const FAudioBufferReadRef& InSignal)
            : InputSignal(InSignal)
            , OutputSignal(FAudioBufferWriteRef::CreateNew(InSettings))
        {
        }

        // Helper function for constructing vertex interface
        static const FVertexInterface& DeclareVertexInterface()
        {
            using namespace TesterNodeNames;

            static const FVertexInterface Interface(
                FInputVertexInterface(
                    TInputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSignal))
                ),
                FOutputVertexInterface(
                    TOutputDataVertexModel<FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputSignal))
                )
            );

            return Interface;
        }

        // Metadata about the node
        static const FNodeClassMetadata& GetNodeInfo()
        {
            auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
            {
                FNodeClassMetadata Metadata;
                Metadata.ClassName = { StandardNodes::Namespace, TEXT("Tester (audio)"), StandardNodes::AudioVariant };
                Metadata.MajorVersion = 1;
                Metadata.MinorVersion = 0;
                Metadata.DisplayName = METASOUND_LOCTEXT("TesterDisplayName", "Tester (audio)");
                Metadata.Description = METASOUND_LOCTEXT("TesterDesc", "Blank node for testing purposes.");
                Metadata.Author = "Charles Matthews";
                Metadata.PromptIfMissing = PluginNodeMissingPrompt;
                Metadata.DefaultInterface = DeclareVertexInterface();
                Metadata.CategoryHierarchy = { METASOUND_LOCTEXT("CustomCategory", "Test") };
                Metadata.Keywords = TArray<FText>(); // Keywords for searching

                return Metadata;
            };

            static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
            return Metadata;
        }

        // Input Data References
        virtual FDataReferenceCollection GetInputs() const override
        {
            using namespace TesterNodeNames;

            FDataReferenceCollection InputDataReferences;
            InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSignal), InputSignal);

            return InputDataReferences;
        }

        // Output Data References
        virtual FDataReferenceCollection GetOutputs() const override
        {
            using namespace TesterNodeNames;

            FDataReferenceCollection OutputDataReferences;
            OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputSignal), OutputSignal);

            return OutputDataReferences;
        }

        // Operator Factory Method
        static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors)
        {
            using namespace TesterNodeNames;

            const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;
            const FInputVertexInterface& InputInterface = DeclareVertexInterface().GetInputInterface();

            // Retrieve the input reference or use defaults
            TDataReadReference<FAudioBuffer> InputSignal = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FAudioBuffer>(
                InputInterface,
                METASOUND_GET_PARAM_NAME(InputSignal),
                InParams.OperatorSettings
            );

            return MakeUnique<FTesterOperator>(InParams.OperatorSettings, InputSignal);
        }

        // Primary node functionality
        virtual void Execute() override
        {
            // Pass the input directly to the output
            int32 NumFrames = InputSignal->Num();
            const float* InputDataPtr = InputSignal->GetData();
            float* OutputDataPtr = OutputSignal->GetData();

            FMemory::Memcpy(OutputDataPtr, InputDataPtr, NumFrames * sizeof(float));
        }

    private:
        // Input References
        FAudioBufferReadRef InputSignal;

        // Output Reference
        FAudioBufferWriteRef OutputSignal;
    };

    // Node Facade Class
    class FTesterNode : public FNodeFacade
    {
    public:
        FTesterNode(const FNodeInitData& InitData)
            : FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FTesterOperator>())
        {
        }
    };

    // Register the Node
    METASOUND_REGISTER_NODE(FTesterNode);
}

#undef LOCTEXT_NAMESPACE