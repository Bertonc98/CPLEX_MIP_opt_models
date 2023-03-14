echo "########################################################## Linearized Model ##########################################################"
bash run_generated_model.sh linearized_model
echo "########################################################## McCormick Model ##########################################################"
bash run_generated_model.sh mccormick_model
echo "########################################################## Quadratic Model ##########################################################"
bash run_generated_model.sh quadratic_model
echo "########################################################## Basic Model ##########################################################"
bash run_generated_model.sh basic_model
echo "########################################################## Stronger Model ##########################################################"
bash run_generated_model.sh stronger_model

