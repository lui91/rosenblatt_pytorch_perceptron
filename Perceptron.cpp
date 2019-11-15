#include "Perceptron.h"
#include <iomanip>

using namespace torch;
using namespace std;

Perceptron::Perceptron(float treshold, float learning_rate, int neurons){
    setTreshold(treshold);
    setLearning_Rate(learning_rate);
    setBias(0.01f);
    torch::Tensor r = torch::rand(neurons);
    setW(r);
}

void Perceptron::fit(int train){
    // 60,000
    if(train){

        auto data_loader = torch::data::make_data_loader(
            torch::data::datasets::MNIST("./data").map(
                torch::data::transforms::Stack<>()
            ),
         torch::data::DataLoaderOptions().batch_size(30).workers(8));
        
        for (torch::data::Example<> &batch : *data_loader)
        {   
            for (int64_t i = 0; i < batch.data.size(0); ++i)
            {
                int label = batch.target[i].item<int64_t>(); 
                if(label == 0 || label == 1) {
                    Tensor sample = batch.data[i].reshape(-1);
                    int prediction = predict(sample);
                    int difference = label - prediction;
                    float weight_update = getLearning_Rate() * difference;
                    setW(getW() + (weight_update * sample));
                    setBias(getBias() + weight_update);
                }
            }
        }
        torch::save(getW(), "weights.pk");
        cout << "Perceptron fitted" << endl;

    }
    else
    {
        cout << "loading W vector" << endl;
        torch::Tensor w;
        torch::load(w, "weights.pk");
        setW(w);
    }
    
}

void Perceptron::test(){
    // 10,000
    auto mode = torch::data::datasets::MNIST::Mode::kTest;
    auto img_test = torch::data::datasets::MNIST("./data", mode).images();
    auto test_targets = torch::data::datasets::MNIST("./data", mode).targets();

    float target_predictions = 0;
    float correct_predictions = 0;
    for (size_t i = 0; i < img_test.size(0); i++)
    {
        if(test_targets[i].item<int64_t>() == 0 || test_targets[i].item<int64_t>() == 1){
            target_predictions =+ target_predictions + 1;
            int prediction = predict(img_test[i].reshape(-1));
            if (prediction == test_targets[i].item<int64_t>()){
                correct_predictions =+ correct_predictions + 1;
            }
        }
    }
    float precision = correct_predictions / target_predictions;
    cout << "correct_predictions: " << correct_predictions << " | target_predictions: " << target_predictions << endl;
    printf("precision %2.5f", precision);
    cout << endl;

}

int Perceptron::predict(Tensor data){
    auto prediction = dot(data, getW()) + getBias();
    if(prediction.item().toFloat() > 0){
        return 1;
    }
    else
    {
        return 0;
    }
}

int Perceptron::relu_derivative(float x){
    if (x <= 0) return 0;
    else if (x > 0) return 1;
}


//Member methods definition
Tensor Perceptron::getW(){
    return w;
}

void Perceptron::setW(Tensor w_value){
    w = w_value;
}

float Perceptron::getTreshold(){
    return treshold;
}

void Perceptron::setTreshold(float treshold_value){
    treshold = treshold_value;
}

float Perceptron::getLearning_Rate(){
    return learning_rate;
}

void Perceptron::setLearning_Rate(float learning_rate_value){
    learning_rate = learning_rate_value;
}

float Perceptron::getBias(){
    return bias;
}

void Perceptron::setBias(float bias_value){
    bias = bias_value;
}
