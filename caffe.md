Reference: http://caffe.berkeleyvision.org/gathered/examples/mnist.html

# prototxt
## Top and Bottom
- layer1: top) data, label
- layer2: bottom) data, top) conv
- layer3: bottom) label, conv

In case of above definition, layer structure is below:
<pre>
[layer1]  +-> data --[layer2]--> conv --+--> [layer3]
          +-> label --------------------+
</pre>

## Data Layer
<pre>
layer {
  name: "mnist" # Layer name
  type: "Data"
  top: "data"
  top: "label"
  data_param {
    source: "examples/mnist/mnist_train_lmdb" # Filepath of input data
    backend: LMDB
    batch_size: 64 # Batch size
  }
  transform_param {
    scale: 0.00390625 # Scale coefficient: by multiplying it, MNIST data range change 0-255 to 0-1
  }
}
</pre>

## Convolution Layer
<pre>
  layer {
    name: "conv1"
    type: "Convolution"
    bottom: "data"
    top: "conv1"
    param { lr_mult: 1 decay_mult: 1 } # learning rate and weight decay for kernel paramter
    param { lr_mult: 2 decay_mult: 0 } #                //                  bias parameter
    convolution_param {
      num_output: 16     # The number of kernel
      kernel_size: 3     # kernel size 3 x 3
      stride: 4          # stride
      weight_filler {
        type: "gaussian" # initialize the filters from a Gaussian
        std: 0.01        # standar deviation in Gaussian destribution
      }
      bias_filler {
        type: "constant" # initialize the biases to zero (0)
        value: 0
      }
    }
  }
</pre>

## Pooling Layer
<pre>
layer {
  name: "pool1"
  type: "Pooling"
  bottom: "conv1"
  top: "pool1"
  pooling_param {
    kernel_size: 2
    stride: 2
    pool: MAX
  }
}
</pre>
## Fully Connected Layer
<pre>
layer {
  name: "ip1"
  type: "InnerProduct"
  bottom: "pool2"
  top: "ip1"
  param { lr_mult: 1 } # lr and weigth decay for kernel
  param { lr_mult: 2 } #           //        for bias
  inner_product_param {
    num_output: 500    # The number of units
    weight_filler {
      type: "xavier"   # Xavier Initializer
    }
    bias_filler {
      type: "constant" # zeror(0) biases
    }
  }
}
</pre>
## Activation Layer
<pre>
layer {
  name: "relu1"
  type: "ReLU"
  bottom: "ip1"
  top: "ip1"
}
</pre>

## Loss Layer
<pre>
layer {
  name: "loss"
  type: "SoftmaxWithLoss"
  bottom: "ip2"
  bottom: "label"
}
</pre>
